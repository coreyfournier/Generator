#pragma once
#include "IO/IBoardIo.h"
#include "IO/IPinChangeListner.h"
#include "IO/Pin.cpp"
#include "PowerDevice.cpp"


using namespace IO;
namespace Devices
{
    /// @brief Some device that supports starting, stopping, and ouputs power
    class StartableDevice :public PowerDevice
    {
        private:
        Pin* _start;        
        Pin* _stop;
        

        public:
        StartableDevice(Pin* l1, Pin* l2, Pin* start, Pin* stop, IO::IBoardIO* board) : PowerDevice(l1, l2, board), _start(start), _stop(stop)
        {
            if(start != nullptr)
                _pins.push_back(*start);

            if(stop != nullptr)
                _pins.push_back(*stop);

        }

        void Start()
        {            
            this->_board->DigitalWrite(*this->_start, true);
            this->_board->Delay(1000);
            this->_board->DigitalWrite(*this->_start, false);
        }

        void Stop()
        {            
            this->_board->DigitalWrite(*this->_stop, true);
            this->_board->Delay(1000);
            this->_board->DigitalWrite(*this->_stop, false);
        }
        
    };
}
