#ifndef PIO_UNIT_TESTING
#pragma once
#include <Arduino.h>
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

        bool DigitalReadAndSet(Pin& pin)
        {
            pin.state = digitalRead(pin.gpio) == HIGH;
            return pin.state;
        }

        void DigitalWrite(Pin& pin, bool value)
        {
            Serial.printf("Writing pin %i value=%i\n", pin.gpio, value);
            pin.state = value;
            digitalWrite(pin.gpio, value? HIGH : LOW);
        }

        void TaskDelay(int milliseconds)
        {
            vTaskDelay(milliseconds);
        }

        void Delay(int milliseconds)
        {
            //Yield then wait
            taskYIELD();
            //Everything should be running in a task anyway
            vTaskDelay(milliseconds);
        }

        uint32_t TicksOfTime()
        {
            return xTaskGetTickCountFromISR();
        }
    };
}
#endif