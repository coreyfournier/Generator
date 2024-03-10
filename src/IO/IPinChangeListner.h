#pragma once
#include "Pin.cpp"

namespace IO
{
    class IPinChangeListner
    {
        public:
        virtual void PinChanged(Pin& pin, bool requiresRead) = 0;    
    };
}