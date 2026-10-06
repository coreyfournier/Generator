#pragma once
#include <stdint.h>
#include "IEvent.cpp"

namespace States
{
    /// @brief Notified each time the state machine processes an event (including sub events).
    /// Called on the state machine task, so implementations must return quickly and never block.
    class IStateChangeListner
    {
        public:
        virtual void OnStateChanged(Event event, uint32_t time) = 0;
    };
}
