#pragma once
#include <string>
#include "Pin.cpp"
#include "IEvent.cpp"
using namespace std;

namespace States
{
    struct ChangeMessage
    {
        Event event;
        Pin pin;
        
        ChangeMessage(){}

        ChangeMessage(Event e, Pin& pin)
        {
            this->pin = pin;
            this->event = e;
        }
    };
}