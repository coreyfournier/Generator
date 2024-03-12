#pragma once
#include "IO/IBoardIo.h"
#include "IO/IPinChangeListner.h"
#include "IO/Pin.cpp"
#include "IO/ISerial.h"
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
        IO::ISerial* _serial;

        public: 
        TransferSwitch(Pin* pin, IO::IBoardIO* board, IO::ISerial* serial) : _pin(pin), _board(board), _serial(serial)
        {
            _pins.push_back(*pin);
        }
        
        /// @brief Toggles the state
        /// @param onOrOff true for on, false for off
        void Toggle(bool onOrOff)
        {
            if(onOrOff)
                this->_serial->Println("Transfering to generator.");
            else
                this->_serial->Println("Transfering to utility.");

            this->_board->DigitalWrite(*this->_pin, onOrOff);

            this->_pin->state = onOrOff;
        }

        std::vector<IO::Pin> GetPins()
        {
            return _pins;
        }
    };

}