#pragma once
#include "IO/IBoardIo.h"
#include "IO/Pin.cpp"
#include "Devices/IPinList.h"

using namespace IO;

namespace Devices
{
    class PowerDevice: public IPinList
    {
        protected:
        Pin* _L1;
        /// @brief L2 is optional. When not set it will be nullptr
        Pin* _L2;
        IO::IBoardIO* _board;
        std::vector<IO::Pin> _pins;

        public:

        /// @brief Constructor
        /// @param l1 
        /// @param l2 optional, nullptr if it doesn't exists
        PowerDevice(Pin* l1, Pin* l2, IO::IBoardIO* board) : _L1(l1), _L2(l2), _board(board)
        {
            _pins.push_back(*l1);

            if(l2 != nullptr)
                _pins.push_back(*l2);
        }

        /// @brief Constructor
        /// @param l1 
        PowerDevice(Pin* l1, IO::IBoardIO* board) : _L1(l1), _L2(nullptr), _board(board)
        {
            _pins.push_back(*l1);
        }

        bool IsOn()
        {
            return this->_L1->state && (this->_L2 == nullptr || this->_L2->state);
        }

        /// @brief Gets and sets the pin state for each leg. Does nothing if the leg is nullptr
        void SetPinState()
        {
            if(this->_L1 != nullptr)
                this->_board->DigitalRead(*this->_L1);            

            if(this->_L2 != nullptr)
                this->_board->DigitalRead(*this->_L2);            
        }  

        std::vector<IO::Pin> GetPins()
        {
            return _pins;
        }      
    };
}