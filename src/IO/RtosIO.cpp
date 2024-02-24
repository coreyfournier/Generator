#pragma once
#include <ArduinoJson.h>
#include "Pin.cpp"
#include "IO/IBoardIo.h"

namespace IO
{
    /// @brief Board specific IO
    class RtosIO : public IBoardIO
    {
        public:
        bool DigitalRead(Pin& pin)
        {
            pin.state = digitalRead(pin.gpio) == HIGH;
            return pin.state;
        }

        void DigitalWrite(Pin& pin, bool value)
        {
            pin.state = value;
            digitalWrite(pin.gpio, value? HIGH : LOW);
        }

        void TaskDelay(int milliseconds)
        {
            vTaskDelay(milliseconds);
        }

        void Delay(int milliseconds)
        {
            delay(milliseconds);
        }
    };
}