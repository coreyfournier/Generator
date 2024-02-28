#pragma once
#include "ChangeMessage.cpp"

class IStateChangeListner
{
    public:
    virtual void StateChange(IEvent event) = 0;    
};