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
        Pin pin;
        
        ChangeMessage(){}

        ChangeMessage(Event e, Pin& pin)
        {
            this->pin = pin;
            this->event = e;
        }
    };
}