#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#include "IO/PowerState.h"
#include "IO/IBoardIo.h"
#include "IPinChangeListner.h"
#include "ChangeMessage.cpp"

using namespace std;

class Orchestration : public IPinChangeListner
{
    private:
    IO::PowerState _powerState;
    IEvent* _listner;
    IO::IBoardIO*  _board;
    std::vector<Pin> _pins;   
    Event _currentEventState = Event::Initalize; 
    QueueHandle_t _queue;
    bool _isUtilityOn = true;

    void UtilityChange(Pin& pin)
    {
        //Need to make sure both are on
        if(pin.state)
        {
            Pin* utilityL1;
            Pin* utilityL2;

            if(pin.role == PinRole::UtilityOnL1)
            {
                utilityL1 = &pin;

                utilityL2 = this->FindByRole(PinRole::UtilityOnL2);
                if(utilityL2 == nullptr)
                    throw std::invalid_argument("Missing pin role for Utility On L2");
            }
            else
            {
                utilityL2 = &pin;

                utilityL1 = this->FindByRole(PinRole::UtilityOnL1);
                if(utilityL1 == nullptr)
                    throw std::invalid_argument("Missing pin role for Utility On L1");                    
            }

            if(utilityL1->state && utilityL2->state)
            {
                //Need to send notification that the utility is now on
                this->_currentEventState = Event::UtilityOn;
                this->_currentEventState = Event::UtilityOnWait;
                ChangeMessage cm = ChangeMessage(Event::UtilityOnWait);
                xQueueSendToBackFromISR(this->_queue, (void *)&cm, NULL);
            }

        }
        //Utility is off. I don't care which leg it is, the house can't function on one.
        else
        {
            this->StateChange(Event::UtilityOff);
            this->StateChange(Event::UtilityOffWait);

            ChangeMessage cm = ChangeMessage(Event::UtilityOffWait);
            
            xQueueSendToBackFromISR(this->_queue, (void *)&cm, NULL);
        }
    }

    void StateChange(Event e)
    {
        Serial.println(IEvent::ToName(e).c_str());
        this->_currentEventState = e;
        Serial.printf("State change '%s' (%i)\n", IEvent::ToName(e).c_str(), e);
    }

        
    public:

    /// @brief Reads the states of the utility pins on startup to see what to do. If they are off, then it fires an event.
    void Initalize()
    {
        this->StateChange(Event::Initalize);

        Pin* utilityL1 = this->FindByRole(PinRole::UtilityOnL1);
        
        if(utilityL1 == nullptr)
            Serial.println("UtilityL1 pin was not found");
        else
        {
            this->_board->DigitalRead(*utilityL1);

            if(!utilityL1->state)
                this->ChangeListner(*utilityL1);
        }

        Pin* utilityL2 = this->FindByRole(PinRole::UtilityOnL2);
        
        if(utilityL2 == nullptr)
            Serial.println("UtilityL2 pin was not found");
        else
        {            
            this->_board->DigitalRead(*utilityL2);
            if(!utilityL2->state)
                this->ChangeListner(*utilityL2);
        }
    }

    void ChangeListner(Pin& pin)
    {
        Serial.printf("ChangeListner %s (%i) State=%i\n", pin.name.c_str(), pin.gpio, pin.state);

        if(pin.role == PinRole::UtilityOnL1 || pin.role == PinRole::UtilityOnL1)
        {
            this->UtilityChange(pin);
        }
        else if(pin.role == PinRole::GeneratorOnL1 || pin.role == PinRole::GeneratorOnL2)
        {
            if(pin.role == PinRole::GeneratorOnL1 && pin.role == PinRole::GeneratorOnL2)
            {
                this->StateChange(Event::GeneratorOn);
            }
            else
            {

            }
        }
        printf("****************Change found %s %i*******************\n", pin.name.c_str(), pin.state);
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

    /// @brief Constructor
    /// @param listner 
    /// @param board 
    Orchestration(
        IEvent* listner,
        IO::IBoardIO* board
    ) :  _listner(listner), _board(board)
    {        
       this->_queue = xQueueCreate(10, sizeof(ChangeMessage));  
    }

    void StateWaiter()
    {
        Serial.printf("Starting StateWaiter\n");

        struct ChangeMessage changeMessage;

        while(true)
        {
            xQueueReceive(this->_queue, &( changeMessage ), portMAX_DELAY);
            
            this->StateChange(changeMessage.event);

            if(changeMessage.event == Event::UtilityOffWait)
            {
                Serial.printf("Delay for utility off\n");
                
                this->_board->TaskDelay(10000);
            }
            else if(changeMessage.event == Event::GeneratorStarting)
            {
                Serial.printf("Waiting during startup\n");

                this->_board->TaskDelay(10000);                
            }
            else if(changeMessage.event == Event::GeneratorWarmUp)
            {
                Serial.printf("Generator warmup\n");

                this->_board->TaskDelay(10000);
                //Can now transfer
            }
        }
    }
    
    
    void DigitalWrite(Pin& pin, bool value)
    {
        if(value != pin.state)
        {
            this->_board->DigitalWrite(pin, value);
            this->ChangeListner(pin);
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