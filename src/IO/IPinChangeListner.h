#pragma once
#include "Pin.cpp"

namespace IO
{
    class IPinChangeListner
    {
        public:
        virtual void PinChange(Pin& pin) = 0;    
    };
}