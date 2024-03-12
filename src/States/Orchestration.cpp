#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#include <map>
#include "IO/IBoardIo.h"
#include "IO/IPinChangeListner.h"
#include "Devices/PowerDevice.cpp"
#include "Devices/StartableDevice.cpp"
#include "Devices/TransferSwitch.cpp"
#include "States/IEvent.cpp"
#include "States/ChangeMessage.cpp"
#include "States/IState.h"
#include "States/UtilityOn.cpp"
#include "States/UtilityOff.cpp"
#include "States/GeneratorStart.cpp"
#include "States/GeneratorOn.cpp"
#include "States/TransferToGenerator.cpp"
#include "States/Initial.cpp"
#include "States/IContext.h"
#include "IO/ISerial.h"
#include "IO/IQueue.h"
#include "Devices/IPinList.h"

#include <stdio.h>

using namespace std;

namespace States
{
    class Orchestration : public IO::IPinChangeListner, public IContext
    {
        private:
        Devices::StartableDevice* _generator;
        Devices::PowerDevice* _utility;
        Devices::TransferSwitch* _transferSwitch;        
        IEvent* _listner;
        std::vector<Pin> _pins;   
        IO::IQueue* _pinQueueChange;
        bool _isUtilityOn = true;
        IState* _currentState;
        /// @brief Current event for the current state. A state can have sub events.
        Event _currentEvent;
        std::map<Event, IState*> _stateMap;
        typedef std::pair<Event, IState*> StatePair;
        IO::ISerial* _serial;
        IO::IBoardIO* _board;

        /// @brief Creates a comprehensive list of all of the pins used.
        /// @param pinList 
        void AddDevicePins(Devices::IPinList* pinList)
        {
            if(pinList != nullptr)
            {
                auto allPins = pinList->GetPins();
                
                this->_serial->Println(IO::string_format("loading pins %i\n", allPins.size()));            

                for(int i=0; i< allPins.size(); i++)
                {
                    this->_serial->Println(IO::string_format("loading pin %s\n", allPins[i].name.c_str()));            
                    this->_pins.push_back(allPins[i]);
                }
            }
        }
            
        public:

        /// @brief Constructor
        /// @param listner 
        /// @param board 
        Orchestration(
            IEvent* listner,
            Devices::PowerDevice* utility,
            Devices::StartableDevice* generator,
            Devices::TransferSwitch* transferSwitch, 
            IO::IBoardIO* board,
            IO::IQueue* queue,
            IO::ISerial* serial
        ) :  _listner(listner), 
            _utility(utility),
            _generator(generator),
            _transferSwitch(transferSwitch),
            _board(board),
            _pinQueueChange(queue),
            _serial(serial)            
        {                    
            
        }

        IO::ISerial* GetSerialIO()
        {
            return this->_serial;
        }

        Devices::PowerDevice* GetUtility()
        {
            return this->_utility;
        }        
        
        Devices::StartableDevice* GetGenerator()
        {
            return this->_generator;
        }

        Devices::TransferSwitch* GetTransferSwitch()
        {
            return this->_transferSwitch;
        }

        /// @brief Gets the current event state
        /// @return 
        Event GetState()
        {
            return this->_currentEvent;
        }

        /// @brief Gets the current event state name
        /// @return 
        string GetStateName()
        {
            return IEvent::ToName(this->_currentEvent);
        }

        /// @brief Causes a wait
        /// @param milliseconds 
        void Delay(int milliseconds)
        {
            this->_board->Delay(milliseconds);
        }

        /// @brief Handles the event when the pin changes. Updates the pin, and will fire an event to wake up the system if necessary
        /// @param pin 
        /// @param requiresRead 
        void PinChanged(Pin& pin, bool requiresRead)
        {
            /**
             * !!!!!!!!!!!!!!This will need some kind of event to trigger a state when the generator is at idle!!!!!!!!!!!!!
             */
            if(requiresRead)
                this->_board->DigitalRead(pin);

            this->_serial->Println(IO::string_format("ChangeListner %s (%i) State=%i\n", pin.name.c_str(), pin.gpio, pin.state));            
        }     

        void StateChange(ChangeMessage& cm)
        {
            this->_serial->Println("Queuing message");
            this->_serial->Println(IO::string_format("State changed '%s' (%i)\n", IEvent::ToName(cm.event).c_str(), cm.event));                        
            this->_pinQueueChange->QueueMessage(cm);
        }

        void StateChange(Event e)
        {
            ChangeMessage cm = ChangeMessage(e, nullptr);
            this->StateChange(cm);
        }   

        /// @brief Reads the states of the utility pins on startup to see what to do. If they are off, then it fires an event.
        void Initalize()
        {   
            auto utilityOn = new UtilityOn(this);        
            auto utilityOff = new UtilityOff(this);
            auto generatorStart = new GeneratorStart(this);
            auto generatorOn = new GeneratorOn(this);
            auto transferToGenerator = new TransferToGenerator(this);

            this->_stateMap.insert(StatePair(Event::Initalize, new Initial(this)));            
            this->_stateMap.insert(StatePair(Event::Utility_On, utilityOn));            
            this->_stateMap.insert(StatePair(Event::Utility_Off, utilityOff));
            this->_stateMap.insert(StatePair(Event::Generator_Start, generatorStart));
            this->_stateMap.insert(StatePair(Event::Generator_On, generatorOn));
            this->_stateMap.insert(StatePair(Event::Transfer_To_Generator, transferToGenerator));
            
            //This is the initalize
            this->SetDevices();
            //this->StateChange(Event::Initalize);        
        }

        void SetDevices()
        {
            this->_currentEvent = Event::Initalize;
            this->_serial->Println("Starting to set devices");

            //Performs the inital read of the state
            this->_utility->SetPinState();
            this->_generator->SetPinState();

            this->AddDevicePins(this->_utility);
            this->AddDevicePins(this->_generator);
            this->AddDevicePins(this->_transferSwitch);

            //It's not on, so trigger the utility off state
            if(!this->_utility->IsOn())
                this->StateChange(Event::Utility_Off);

            this->_serial->Println("Done setting devices");
        } 

        /// @brief Gets the total number of pins
        /// @return 
        int PinCount()
        {            
            return this->_pins.size();
        }

        /// @brief Gets the pin by index. Necessary for looping through them
        /// @param index 
        /// @return 
        Pin GetPin(int index)
        {
            return this->_pins[index];
        }    

        /// @brief Waits for pin change events and sets the pin values 
        void WaitAndListen()
        {
            this->_serial->Println(IO::string_format("Starting Initalize\n"));
            this->Initalize();
            
            this->_serial->Println(IO::string_format("Starting StateWaiter\n"));

            struct ChangeMessage changeMessage;

            while(true)
            {
                this->_serial->Println(IO::string_format("WaitAndListen loop"));
                changeMessage = this->_pinQueueChange->BlockAndDequeue();
                this->_currentEvent = changeMessage.event;
                
                this->_serial->Println(IO::string_format("Message found, starting to process %s ......", IEvent::ToName(this->_currentEvent).c_str()));                                
                            

                if(this->_stateMap[changeMessage.event] != nullptr)
                {
                    this->_currentState = this->_stateMap[changeMessage.event];            

                    this->_serial->Println(IO::string_format("Current State GetName(): '%s'", this->_currentState->GetName().c_str()));
                    
                    this->_currentState->DoAction();
                }
                else
                {
                    Event e = changeMessage.event;
                    this->_serial->Println(IO::string_format(
                        "No state map for '%s' (%i) this is probably a substate for the current state\n", 
                        IEvent::ToName(e).c_str(), 
                        e));                        
                }
            }
        }    
        
        void DigitalWrite(Pin& pin, bool value)
        {
            if(value != pin.state)
            {
                this->_board->DigitalWrite(pin, value);
                this->PinChanged(pin, false);
            }
            else
            {
                this->_serial->Println(IO::string_format("No Change\n"));
            }

            pin.state = value;
        }

        bool DigitalRead(Pin& pin)
        {
            return this->_board->DigitalRead(pin);
        }

        /// @brief Looks for the specified pin in the list. If not found nullptr is returned.
        /// @param gpio 
        /// @return Pin or nullptr
        Pin* FindByGpio(int gpio)
        {
            for(int i=0; i< this->PinCount(); i++)
            {
                if(gpio == this->GetPin(i).gpio)
                    return &this->_pins[i];
            }

            return nullptr;
        }

        /// @brief Finds the pin by the role. Returns nullptr if not found
        /// @param role 
        /// @return 
        Pin* FindByRole(PinRole role)
        {
            for(int i=0; i< this->PinCount(); i++)
            {
                if(role == this->GetPin(i).role)
                    return &this->_pins[i];
            }

            return nullptr;
        }
    };
}