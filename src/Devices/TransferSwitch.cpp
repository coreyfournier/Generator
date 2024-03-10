#pragma once
#include "IO/IBoardIo.h"
#include "IO/IPinChangeListner.h"
#include "IO/Pin.cpp"
#include "IO/IBoardIo.h"
#include "Devices/IPinList.h"

using namespace IO;

namespace Devices
{
    class TransferSwitch : public IPinList
    {
        private:
        Pin* _pin;
        IO::IBoardIO* _board;
        std::vector<IO::Pin> _pins;

        public: 
        TransferSwitch(Pin* pin, IO::IBoardIO* board) : _pin(pin), _board(board)
        {
            _pins.push_back(*pin);
        }
        
        /// @brief Toggles the state
        /// @param onOrOff true for on, false for off
        void Toggle(bool onOrOff)
        {

        }

        std::vector<IO::Pin> GetPins()
        {
            return _pins;
        }
    };

}