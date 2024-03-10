#pragma once
#include "IO/Pin.cpp"
#include "IO/ISerial.h"
#include "Devices/PowerDevice.cpp"
#include "States/IEvent.cpp"

using namespace IO;

namespace States
{
    class IContext
    {
        public:
        /// @brief Looks for the specified pin in the list. If not found nullptr is returned.
        /// @param gpio 
        /// @return Pin or nullptr
        virtual Pin* FindByGpio(int gpio) = 0;

        /// @brief Finds the pin by the role. Returns nullptr if not found
        /// @param role 
        /// @return 
        virtual Pin* FindByRole(PinRole role) = 0;

        virtual IO::ISerial* GetSerialIO() = 0;

        /// @brief Changes the state
        /// @param e 
        virtual void StateChange(Event e) = 0;

        /// @brief Gets the utility control. nullptr if not found or set
        /// @return 
        virtual Devices::PowerDevice* GetUtility() = 0;
        
        /// @brief Gets the generator control. nullptr if not found or set
        /// @return 
        virtual Devices::PowerDevice* GetGenerator() = 0;

        /// @brief Causes a wait
        /// @param milliseconds 
        virtual void Delay(int milliseconds) = 0;
    };
}