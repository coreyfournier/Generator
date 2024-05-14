#pragma once
#include "IO/IBoardIo.h"
#include "IO/IPinChangeListner.h"
#include "IO/Pin.cpp"
#include "PowerDevice.cpp"
#include "config.h"

using namespace IO;

namespace Devices
{
    /// @brief Some device that supports starting, stopping, and ouputs power
    class StartableDevice :public PowerDevice
    {
        private:
        Pin* _start;        
        Pin* _stop;
        bool _isMomentarySwitch;

        void Toggle(Pin& pin, int momentaryDelay)
        {           
            if(this->_isMomentarySwitch)
            {
                this->_board->DigitalWrite(pin, true);
                this->_board->TaskDelay(momentaryDelay);
                this->_board->DigitalWrite(pin, false);
            }
            else
            {
                auto current = this->_board->DigitalRead(pin);

                this->_board->DigitalWrite(pin, !current);
            }
        }
        

        public: 
        /// @brief 
        /// @param l1 Sense voltage from leg 1 Required
        /// @param l2 Sense voltage from leg 2 Not required
        /// @param start Pin to start or stop the device
        /// @param stop If null, then the start pin is used.
        /// @param board Board to interface with
        /// @param isMomentarySwitch Turning on or off the device is on, delayed, then off.
        StartableDevice(Pin* l1, Pin* l2, Pin* start, Pin* stop, IO::IBoardIO* board, bool isMomentarySwitch) : PowerDevice(l1, l2, board), _start(start), _stop(stop), _isMomentarySwitch(isMomentarySwitch)
        {
            if(start != nullptr)
                _pins.push_back(*start);

            if(stop != nullptr)
                _pins.push_back(*stop);
        }

        void Start()
        {            
            this->Toggle(*this->_start, DefaultTimeToTriggerStart);
        }

        void Stop()
        {            
            if(this->_stop == nullptr)
                this->Toggle(*this->_start, DefaultTimeToTriggerStop);
            else
                this->Toggle(*this->_stop, DefaultTimeToTriggerStop);
        }
        
    };
}
