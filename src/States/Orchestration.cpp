#pragma once
#include <string>
#include <vector>
#include <list>
#include <stdint.h>
#include <map>
#include "IO/IBoardIo.h"
#include "IO/IPinChangeListner.h"
#include "Devices/PowerDevice.cpp"
#include "Devices/StartableDevice.cpp"
#include "Devices/TransferSwitch.cpp"
#include "States/IEvent.cpp"
#include "States/Initalize.cpp"
#include "States/ChangeMessage.cpp"
#include "States/PinChange.cpp"
#include "States/IState.h"
#include "States/UtilityOn.cpp"
#include "States/UtilityOff.cpp"
#include "States/GeneratorStart.cpp"
#include "States/GeneratorOn.cpp"
#include "States/GeneratorStop.cpp"
#include "States/TransferToGenerator.cpp"
#include "States/TransferToUtility.cpp"
#include "States/Idle.cpp"
#include "States/Disabled.cpp"
#include "States/IContext.h"
#include "IO/ISerial.h"
#include "IO/IQueue.h"
#include "Devices/IPinList.h"
#include "IO/RingBuffer.cpp"

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
        std::vector<Pin> _pins;   
        IO::IQueue<States::ChangeMessage>* _stateQueueChange;
        IO::IQueue<States::PinChange>* _pinQueueChange;
        bool _isUtilityOn = true;
        IState* _currentState = nullptr;
        /// @brief Current event for the current state. A state can have sub events.
        Event _currentEvent;
        std::map<Event, IState*> _stateMap;
        typedef std::pair<Event, IState*> StatePair;
        typedef std::pair<uint32_t, int> GpioTimePair;
        IO::ISerial* _serial;
        IO::IBoardIO* _board;
        IO::RingBuffer<Event>* _lastEvents  = new IO::RingBuffer<Event>(20);

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
        Orchestration(
            Devices::PowerDevice* utility,
            Devices::StartableDevice* generator,
            Devices::TransferSwitch* transferSwitch, 
            IO::IBoardIO* board,
            IO::IQueue<States::ChangeMessage>* stateQueue,
            IO::IQueue<States::PinChange>* pinQueue,
            IO::ISerial* serial
        ) :  
            _utility(utility),
            _generator(generator),
            _transferSwitch(transferSwitch),
            _board(board),
            _stateQueueChange(stateQueue),
            _pinQueueChange(pinQueue),
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
            this->_board->TaskDelay(milliseconds);
        }

        /// @brief Handles the event when the pin changes.
        /// @param pin 
        /// @param requiresRead set to true when the pin value needs to be read and set from the board. False if the system is setting the value.
        void PinChanged(Pin& pin, bool requiresRead)
        {
            this->_pinQueueChange->QueueMessage(
                new PinChange(&pin, this->_board->TicksOfTime(), requiresRead)
            );
        }     

        void StateChange(ChangeMessage* cm)
        {
            this->_serial->Println("Queuing message");
            this->_serial->Println(IO::string_format("State changed '%s' (%i)\n", IEvent::ToName(cm->event).c_str(), cm->event));                        
            this->_lastEvents->Add(cm->event);
            this->_stateQueueChange->QueueMessage(cm);
        }

        void StateChange(Event e)
        {            
            this->StateChange(new ChangeMessage(e, this->_board->TicksOfTime()));
        }   

        /// @brief Reads the states of the utility pins on startup to see what to do. If they are off, then it fires an event.
        void Initalize()
        {   
            this->_currentEvent = Event::Initalize;

            auto utilityOn = new UtilityOn(this);        
            auto utilityOff = new UtilityOff(this);
            auto generatorStart = new GeneratorStart(this);
            auto generatorOn = new GeneratorOn(this);
            auto transferToGenerator = new TransferToGenerator(this);

            this->_stateMap.insert(StatePair(Event::Initalize, new States::Initalize(this)));
            this->_stateMap.insert(StatePair(Event::Utility_On, utilityOn));            
            this->_stateMap.insert(StatePair(Event::Utility_Off, utilityOff));
            this->_stateMap.insert(StatePair(Event::Generator_Start, generatorStart));
            this->_stateMap.insert(StatePair(Event::Generator_Stop, new GeneratorStop(this)));
            this->_stateMap.insert(StatePair(Event::Generator_On, generatorOn));
            this->_stateMap.insert(StatePair(Event::Transfer_To_Generator, transferToGenerator));
            this->_stateMap.insert(StatePair(Event::Transfer_To_Utility, new TransferToUtility(this)));
            this->_stateMap.insert(StatePair(Event::Idle, new States::Idle(this)));
            this->_stateMap.insert(StatePair(Event::Disabled, new States::Disabled(this)));
            
            this->SetDevices();
            this->StateChange(Event::Disabled);        
        }

        void SetDevices()
        {            
            this->_serial->Println("Starting to set devices");

            //Performs the inital read of the state
            this->_utility->SetPinState();
            this->_generator->SetPinState();

            this->AddDevicePins(this->_utility);
            this->AddDevicePins(this->_generator);
            this->AddDevicePins(this->_transferSwitch);

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

        void WaitAndListenForPinChanges()
        {
            std::map<int, uint32_t> gpioLastChanged;
            PinChange* changeMessage;

            this->_serial->Println(IO::string_format("Starting Initalize\n"));
            this->Initalize();            
            this->_serial->Println(IO::string_format("Starting StateWaiter\n"));

            while(true)
            {
                //this->_serial->Println(IO::string_format("WaitAndListen loop"));
                changeMessage = this->_pinQueueChange->BlockAndDequeue();

                this->_board->TaskDelay(100);
                
                if (gpioLastChanged.find(changeMessage->pin->gpio) == gpioLastChanged.end()) {
                    //When initalizing it make sure it will be at least the minimum
                    gpioLastChanged.insert(GpioTimePair(changeMessage->pin->gpio, 0));
                } 
                auto lastChange = gpioLastChanged.find(changeMessage->pin->gpio);
                auto timeDiff = this->_board->TicksOfTime() - lastChange->second;
                
                //It's with in range update the last time.
                if(timeDiff > MinTimeAllowedBetweenMessages) 
                {
                    //Update the last time it was looked at.
                    lastChange->second = this->_board->TicksOfTime();

                    bool pinState = changeMessage->pin->state; 
                    bool notifyChange = true;

                    if(changeMessage->readAndSet)
                    {
                        pinState = this->_board->DigitalRead(*changeMessage->pin);
                        //Only fire a message if it actually changed.
                        notifyChange = (changeMessage->pin->state != pinState);
                        changeMessage->pin->state = pinState;
                    }                            
                                            
                    this->_serial->Println(IO::string_format(
                        "Pin Change %s (%i) State=%i Timediff=%i Notify=%i CurrentEvent=%s\n", 
                        changeMessage->pin->name.c_str(), 
                        changeMessage->pin->gpio, 
                        changeMessage->pin->state,
                        timeDiff,
                        notifyChange,
                        IEvent::ToName(this->_currentEvent).c_str()));         

                    //Only notify the change if it's idle. Each state knows to check for the generator and utility
                    if(notifyChange && (this->_stateMap[Event::Idle] == this->_currentState))
                    {
                        if(changeMessage->pin->role == PinRole::UtilityOnL1 || changeMessage->pin->role == PinRole::UtilityOnL2)
                            if(changeMessage->pin->state)
                                this->StateChange(Event::Utility_On);
                            else
                                this->StateChange(Event::Utility_Off);
                    }
                }
                else
                {
                    //this->_serial->Println(IO::string_format("Event %s was too close together. Exiting", IEvent::ToName(this->_currentEvent).c_str()));     
                }
            }

            delete changeMessage;
        }

        /// @brief Waits for pin change events and sets the pin values 
        void WaitAndListenForStateChanges()
        {
            ChangeMessage* changeMessage;

            while(true)
            {
                //this->_serial->Println(IO::string_format("WaitAndListen loop"));
                changeMessage = this->_stateQueueChange->BlockAndDequeue();
                this->_currentEvent = changeMessage->event;                
                                    
                this->_serial->Println(IO::string_format("Message found, starting to process %s ......", IEvent::ToName(this->_currentEvent).c_str()));                                                                

                if(this->_stateMap[changeMessage->event] != nullptr)
                {
                    this->_currentState = this->_stateMap[changeMessage->event];            

                    this->_serial->Println(IO::string_format("Current State GetName(): '%s'", this->_currentState->GetName().c_str()));
                    
                    this->_currentState->DoAction();
                }
                else
                {
                    Event e = changeMessage->event;
                    this->_serial->Println(IO::string_format(
                        "No state map for '%s' (%i) this is probably a substate for the current state\n", 
                        IEvent::ToName(e).c_str(), 
                        e));                        
                }
                
            }

            delete changeMessage;
        }    
        
        void DigitalWrite(Pin& pin, bool value)
        {
            this->_serial->Println(IO::string_format("DigitalWrite pin %i %i to %i", pin.gpio, pin.state, value));
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

        const std::list<Event> GetLastEvents()
        {
            return this->_lastEvents->GetBuffer();
        }
    };
}