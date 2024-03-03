#pragma once
#include "Pin.cpp"
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
    };
}