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
            return digitalRead(pin.gpio) == HIGH;
        }

        void DigitalWrite(Pin& pin, bool value)
        {
            digitalWrite(pin.gpio, value? HIGH : LOW);
        }
    };
}