#pragma once
#include <vector>
#include "Pin.cpp"
#include "IBoardIo.h"
#include "IPinStateListner.h"

namespace IO
{
    /// @brief Wraps a board and tells listeners about every pin written through it, so output changes
    /// (start/stop pulses, the transfer switch, writes from the web API) are reported no matter which
    /// device makes them. Reads are passed straight through; input changes are reported by Orchestration.
    class NotifyingBoard : public IBoardIO
    {
        private:
        IBoardIO* _inner;
        std::vector<IPinStateListner*> _listeners;

        public:
        NotifyingBoard(IBoardIO* inner) : _inner(inner) {}

        /// @brief Add listeners before any task writes pins; the list is not thread safe.
        void AddListener(IPinStateListner* listener)
        {
            _listeners.push_back(listener);
        }

        void DigitalWrite(Pin& pin, bool value)
        {
            _inner->DigitalWrite(pin, value);

            for(size_t i = 0; i < _listeners.size(); i++)
                _listeners[i]->OnPinChanged(pin);
        }

        bool DigitalRead(Pin& pin)
        {
            return _inner->DigitalRead(pin);
        }

        bool DigitalReadAndSet(Pin& pin)
        {
            return _inner->DigitalReadAndSet(pin);
        }

        void TaskDelay(int milliseconds, bool yieldToo = false)
        {
            _inner->TaskDelay(milliseconds, yieldToo);
        }

        void Delay(int milliseconds)
        {
            _inner->Delay(milliseconds);
        }

        uint32_t TicksOfTime()
        {
            return _inner->TicksOfTime();
        }
    };
}
