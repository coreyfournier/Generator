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
        //Pin* pin;
        /// @brief time of the event
        uint32_t time;

        
        ChangeMessage(){}

        /// @brief 
        /// @param e 
        ChangeMessage(Event e, uint32_t time)
        {
            this->event = e;
            this->time = time;
        }
    };
}