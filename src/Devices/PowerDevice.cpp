#pragma once
#include "IO/IBoardIo.h"
#include "IPinChangeListner.h"

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

        void SetPinState(IO::IBoardIO* board)
        {
            board->DigitalRead(*this->_L2);            

            if(this->_L2 != nullptr)
                board->DigitalRead(*this->_L2);            
        }
        
    };
}