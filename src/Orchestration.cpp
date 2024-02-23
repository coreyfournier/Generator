#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#include "IO/PowerState.h"
#include "IO/IBoardIo.h"
#include "IPinChangeListner.h"

using namespace std;

enum Event { 
    UtilityOn, 
    UtilityOff, 
    GeneratorOn, 
    GeneratorOff, 
    GeneratorStarting, 
    GeneratorCoolingDown, 
    GeneratoredStopping,
    TransferToUtility,
    TransferToGenerator,
    StartFailure
};

class IEvent
{
    private:
   
    public:
    virtual void Change(Event e) = 0;

    static string ToName(Event e)
    {
         static string _enumStrings[10] = { 
        "Utility On",
        "Utility Off",
        "GeneratorOn",
        "GeneratorOff",
        "GeneratorStarting",
         "GeneratorCoolingDown",
        "GeneratoredStopping",
         "TransferToUtility",
         "TransferToGenerator",
         "StartFailure"
         };
        return _enumStrings[(int)e];
    }
};


class Orchestration : public IPinChangeListner
{
    private:
    IO::PowerState _powerState;
    IEvent* _listner;
    IO::IBoardIO*  _board;
    std::vector<Pin> _pins;    
    
    public:

    void ChangeListner(Pin& pin)
    {
        printf("****************Change found %s %i*******************\n", pin.name.c_str(), pin.state);
    }

    /// @brief Adds a new pin to the list
    /// @param pin 
    void AddPin(Pin pin)
    {
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

    Orchestration(
        IEvent* listner,
        IO::IBoardIO* board
    ) :  _listner(listner), _board(board)
    {        
        
    }
    
    void SenseChanges()
    {      
        /*
        if(_utilitySense->IsChanged())
        {
            if(_utilitySense->IsOn())
                _listner->Change(Event::UtilityOn);
            else
                _listner->Change(Event::UtilityOff);
        }

        if(_generatorSense->IsChanged())
        {
            if(_generatorSense->IsOn())
                _listner->Change(Event::GeneratorOn);
            else
                _listner->Change(Event::GeneratorOff);
        }    
        */             
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
};