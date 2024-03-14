#pragma once
#include "Pin.cpp"
#include "IBoardIo.h"
#include <iostream>
#include <chrono>
#include <ctime>  

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

        bool DigitalReadAndSet(Pin& pin)
        {
            return true;
        }

        void TaskDelay(int milliseconds)
        {

        }
        
        void Delay(int milliseconds)
        {

        }

        uint32_t TicksOfTime()
        {
            return 500;
        }
    };
}