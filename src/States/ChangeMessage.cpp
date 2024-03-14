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
        /// @brief time of the event
        uint32_t time;

        
        ChangeMessage(){}

        /// @brief 
        /// @param e 
        /// @param pin set to nullptr if this is just a state change with out a specific pin
        ChangeMessage(Event e, Pin* pin, uint32_t time)
        {
            this->pin = pin;
            this->event = e;
            this->time = time;
        }
    };
}