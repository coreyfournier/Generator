#pragma once
#include "Pin.cpp"
#include "IBoardIo.h"

namespace IO
{
    class MockBoard: public IBoardIO
    {
        public :

        void DigitalWrite(Pin& pin, bool value)
        {

        }

        bool DigitalRead(Pin& pin)
        {
            return false;
        }

        void TaskDelay(int milliseconds)
        {

        }
        
        void Delay(int milliseconds)
        {

        }
    };
}