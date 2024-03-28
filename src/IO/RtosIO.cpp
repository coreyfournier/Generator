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

        /// @brief 
        /// @param gpioPin 
        /// @param mode OUTPUT, PULLUP, INPUT_PULLUP, PULLDOWN, INPUT_PULLDOWN, OPEN_DRAIN, OUTPUT_OPEN_DRAIN, ANALOG
        /// @param defaultTo 
        void SetPinMode(uint8_t gpioPin, uint8_t mode, bool defaultTo)
        {
            pinMode(gpioPin, mode);
            digitalWrite(gpioPin, defaultTo? HIGH : LOW);
        }

        /// @brief 
        /// @param gpioPin 
        /// @param defaultTo 
        void SetPinMode(uint8_t gpioPin, uint8_t mode)
        {
            pinMode(gpioPin, mode);
        }

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

        void TaskDelay(int milliseconds, bool yieldToo = false)
        {
            if(yieldToo)
                taskYIELD();
            vTaskDelay(milliseconds);
        }

        void Delay(int milliseconds)
        {
            //Everything should be running in a task anyway
            delay(milliseconds);
        }

        uint32_t TicksOfTime()
        {
            return xTaskGetTickCountFromISR();
        }
    };
}
#endif