#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#include "IO/IBoardIo.h"
#include "IPinChangeListner.h"
#include "States/ChangeMessage.cpp"

using namespace States;

namespace Devices
{
    class Generator : IPinChangeListner
    {
        private:
        Pin* _L1;
        /// @brief L2 is optional. When not set it will be nullptr
        Pin* _L2;

        public:
        
        /// @brief Constructor
        /// @param l1 
        Generator(Pin& l1)
        {            
            this->_L1 = &l1;
            this->_L2 = nullptr;
        }

        /// @brief Constructor
        /// @param l1 
        /// @param l2 
        Generator(Pin& l1, Pin& l2) : _L1(&l1), _L2(&l2)
        {

        }

       

        void PinChange(Pin& pin)
        {
            if(pin.role == PinRole::GeneratorOnL1 || pin.role == PinRole::GeneratorOnL2)
            {
                if(pin.role == PinRole::GeneratorOnL1 && pin.role == PinRole::GeneratorOnL2)
                {
                    //this->StateChange(Event::GeneratorOn);
                }
                else
                {

                }
            }
        }
    };
}