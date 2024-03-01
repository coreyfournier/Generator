#pragma once
#include "States/ChangeMessage.cpp"
#include "States/IEvent.cpp"
using namespace States;

class IStateChangeListner
{
    public:
    virtual void StateChange(IEvent& event) = 0;    
};