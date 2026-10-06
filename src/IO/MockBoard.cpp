#pragma once
#include "Pin.cpp"
#include "IBoardIo.h"
#include <iostream>
#include <map>

namespace IO
{
    class MockBoard: public IBoardIO
    {
        private:
        std::map<int, bool> _pinValues;
        std::map<int, bool> _writtenValues;
        uint32_t _ticks = 1000;

        public :

        void SetPinValue(int gpio, bool value)
        {
            _pinValues[gpio] = value;
        }

        bool GetWrittenValue(int gpio)
        {
            if(_writtenValues.find(gpio) != _writtenValues.end())
                return _writtenValues[gpio];
            return false;
        }

        void DigitalWrite(Pin& pin, bool value)
        {
            _writtenValues[pin.gpio] = value;
            pin.state = value;
        }

        bool DigitalRead(Pin& pin)
        {
            if(_pinValues.find(pin.gpio) != _pinValues.end())
                return _pinValues[pin.gpio];
            return pin.state;
        }

        bool DigitalReadAndSet(Pin& pin)
        {
            bool val = DigitalRead(pin);
            pin.state = val;
            return val;
        }

        void TaskDelay(int milliseconds, bool yieldToo = false)
        {
            _ticks += milliseconds;
        }

        void Delay(int milliseconds)
        {
            _ticks += milliseconds;
        }

        uint32_t TicksOfTime()
        {
            return _ticks;
        }

        void AdvanceTicks(uint32_t amount)
        {
            _ticks += amount;
        }
    };
}
