#pragma once
#include "IO/IBoardIo.h"
#include "IO/IPinChangeListner.h"
#include "IO/Pin.cpp"

using namespace IO;

namespace Devices
{
    class PowerDevice
    {
        private:
        Pin* _L1;
        /// @brief L2 is optional. When not set it will be nullptr
        Pin* _L2;

        public:

        /// @brief Constructor
        /// @param l1 
        /// @param l2 optional, nullptr if it doesn't exists
        PowerDevice(Pin* l1, Pin* l2, IPinChangeListner* o) : _L1(l1), _L2(l2)
        {

        }

        bool IsOn()
        {
            return this->_L1->state && (this->_L2 == nullptr || this->_L2->state);
        }

        /// @brief Gets and sets the pin state for each leg. Does nothing if the leg is nullptr
        /// @param board 
        void SetPinState(IO::IBoardIO* board)
        {
            if(this->_L1 != nullptr)
                board->DigitalRead(*this->_L1);            

            if(this->_L2 != nullptr)
                board->DigitalRead(*this->_L2);            
        }
        
    };
}