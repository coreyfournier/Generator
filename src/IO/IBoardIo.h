#pragma once
#include "Pin.cpp"

namespace IO
{
    /// @brief Hardware interface
    class IBoardIO
    {
        public :
        /// @brief Write outs the value. Updates the pins state too.
        /// @param pin 
        /// @param value 
        virtual void DigitalWrite(Pin& pin, bool value) = 0;

        /// @brief Reads the pin. Updates the state of the pin too.
        /// @param pin 
        /// @return 
        virtual bool DigitalRead(Pin& pin) = 0;

        virtual void TaskDelay(int milliseconds) = 0;

        virtual void Delay(int milliseconds) = 0;

    };
}