#pragma once
#include "ChangeMessage.cpp"
#include "IEvent.cpp"

namespace States
{
    class IStateChangeListner
    {
        public:
        virtual void StateChange(IEvent& event) = 0;    
    };
}