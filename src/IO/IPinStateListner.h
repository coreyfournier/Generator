#pragma once
#include "Pin.cpp"

namespace IO
{
    /// @brief Notified each time a pin's state changes: a debounced input change (utility or generator
    /// sense) or a write to an output (start/stop, transfer).
    /// Called on whichever task made the change, so implementations must return quickly and never block.
    class IPinStateListner
    {
        public:
        virtual void OnPinChanged(const Pin& pin) = 0;
    };
}
