#pragma once
#include <string>
#include "IO/Pin.cpp"
#include "States/IEvent.cpp"
using namespace std;
using namespace IO;

namespace States
{
    struct ChangeMessage
    {
        Event event;
        Pin* pin;
        
        ChangeMessage(){}

        /// @brief 
        /// @param e 
        /// @param pin set to nullptr if this is just a state change with out a specific pin
        ChangeMessage(Event e, Pin* pin)
        {
            this->pin = pin;
            this->event = e;
        }
    };
}