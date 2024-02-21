#pragma once
#include "Pin.cpp"

namespace IO
{
    /// @brief Hardware interface
    class IBoardIO
    {
        public :
        virtual void DigitalWrite(Pin& pin, bool value) = 0;

        virtual bool DigitalRead(Pin& pin) = 0;
    };
}