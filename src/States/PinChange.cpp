#pragma once
#include <string>
#include "IO/Pin.cpp"
#include "States/IEvent.cpp"
using namespace std;
using namespace IO;

namespace States
{
    struct PinChange
    {
        Pin* pin;
        /// @brief time of the event
        uint32_t time;
        
        /// @brief Should the value be read and set. This is necessary for an interupt event. False when it is set by a user.
        bool readAndSet;
        
        PinChange(){}

        /// @brief 
        /// @param pin 
        /// @param time Time the event occurred
        /// @param readAndSet Should the value be read and set. This is necessary for an interupt event. False when it is set by a user.
        PinChange(Pin* pin, uint32_t time, bool readAndSet)
        {
            this->pin = pin;
            this->readAndSet = readAndSet;
            this->time = time;
        }
    };
}