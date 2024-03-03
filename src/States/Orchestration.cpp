#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#include <map>
#include "IO/PowerState.h"
#include "IO/IBoardIo.h"
#include "IPinChangeListner.h"
#include "ChangeMessage.cpp"
#include "Devices/Generator.cpp"
#include "Devices/Utility.cpp"
#include "IEvent.cpp"
#include "ChangeMessage.cpp"
#include "IState.h"
#include "States/UtilityOn.cpp"
#include "States/UtilityOff.cpp"
#include "States/Initial.cpp"
#include "States/IContext.h"

using namespace std;

namespace States
{
    class Orchestration : public IPinChangeListner, public IContext
    {
        private:
        Devices::Generator* _generator;
        Devices::Generator* _utility;
        IO::PowerState _powerState;
        IEvent* _listner;
        IO::IBoardIO*  _board;
        std::vector<Pin> _pins;   
        QueueHandle_t _pinQueueChange;
        bool _isUtilityOn = true;
        IState* _currentState;
        std::map<Event, IState*> _stateMap;
        typedef std::pair<Event, IState*> StatePair;
            
        public:
        /// @brief Constructor
        /// @param listner 
        /// @param board 
        Orchestration(
            IEvent* listner,
            IO::IBoardIO* board
        ) :  _listner(listner), 
            _board(board)            
        {                    
            this->_pinQueueChange = xQueueCreate(10, sizeof(ChangeMessage));  
            //Register the states
        }

        // /// @brief Gets the current event state
        // /// @return 
        // Event GetState()
        // {
        //     return this->_currentEventState;
        // }


        void QueueMessage(ChangeMessage& cm)
        {
            xQueueSendToBackFromISR(this->_pinQueueChange, (void *)&cm, NULL);
        }
        

        /// @brief Reads the states of the utility pins on startup to see what to do. If they are off, then it fires an event.
        void Initalize()
        {           
            this->_stateMap.insert(StatePair(Event::Initalize, new Initial(this)));
            this->_stateMap.insert(StatePair(Event::Utility_On, new UtilityOn(this)));
            this->_stateMap.insert(StatePair(Event::Utility_Off, new UtilityOff(this)));
            
            this->StateChange(Event::Initalize);         

            this->SetDevices();   
        }

        void SetDevices()
        {
            Pin* generatorL1 = this->FindByRole(PinRole::GeneratorOnL1);
            Pin* generatorL2 = this->FindByRole(PinRole::GeneratorOnL2);

            if(generatorL1 == nullptr)
                throw invalid_argument ("No pin roles found for the generator");
            
            this->_generator = new Devices::Generator(generatorL1, generatorL2, this);            

            Pin* utilityL1 = this->FindByRole(PinRole::UtilityOnL1);
            Pin* utilityL2 = this->FindByRole(PinRole::UtilityOnL2);

            if(utilityL1 == nullptr)
                throw invalid_argument ("No pin roles found for the generator");
            
            this->_utility = new Devices::Generator(utilityL1, utilityL2, this); 

            //Performs the inital read of the state
            this->_utility->SetPinState(this->_board);
            this->_generator->SetPinState(this->_board);                      

            //It's not on, so trigger the utility off state
            if(!this->_utility->IsOn())
                this->StateChange(Event::Utility_Off);
        }
        
        void StateChange(Event e)
        {
            Serial.println(IEvent::ToName(e).c_str());
            this->_currentState = this->_stateMap[e];
            Serial.printf("State change '%s' (%i)\n", IEvent::ToName(e).c_str(), e);

            this->_currentState->DoAction();
        }        

        void PinChange(Pin& pin)
        {            
            Serial.printf("ChangeListner %s (%i) State=%i\n", pin.name.c_str(), pin.gpio, pin.state);

            if((pin.role == PinRole::UtilityOnL1 || pin.role == PinRole::UtilityOnL2))
            {
                if(pin.state)
                    this->StateChange(Event::Utility_On);
                else
                    this->StateChange(Event::Utility_Off);
            }            
        }

        /// @brief Adds a new pin to the list
        /// @param pin 
        void AddPin(Pin pin)
        {
            Serial.printf("Adding pin %s Role:%i\n", pin.name.c_str(), pin.role);
            this->_pins.push_back(pin);
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

        void StateWaiter()
        {
            Serial.printf("Starting StateWaiter\n");

            struct ChangeMessage changeMessage;

            while(true)
            {
                xQueueReceive(this->_pinQueueChange, &( changeMessage ), portMAX_DELAY);
                
                this->StateChange(changeMessage.event);

                
            }
        }    
        
        void DigitalWrite(Pin& pin, bool value)
        {
            if(value != pin.state)
            {
                this->_board->DigitalWrite(pin, value);
                this->PinChange(pin);
            }
            else
                Serial.printf("No Change\n");

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