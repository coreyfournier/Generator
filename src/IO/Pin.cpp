#pragma once
#include <string>
#include "PinRole.cpp"

namespace  IO
{
    class Pin{
        public:
        /// @brief GPIO number on the board
        int gpio;
        /// @brief The pin is either high/ True or low /False
        bool state;
        /// @brief Name to display for this pin
        std::string name; 
        /// @brief If it's readonly then the pin can't be set
        bool isReadOnly;

        /// @brief Identifies what the pin is being used for. This is necessary to so special logic can be used in code when actions are performed against it.
        PinRole role;
        
        Pin(){};

        Pin(int gpio, bool state, std::string name, PinRole role)
        {
            this->gpio = gpio;
            this->state = state;
            this->name = name;
            this->isReadOnly = false;
            this->role = role;
        };

        Pin(int gpio, bool state, std::string name, bool isReadOnly, PinRole role)
        {
            this->gpio = gpio;
            this->state = state;
            this->name = name;
            this->isReadOnly = isReadOnly;
            this->role = role;
        };
    };

}