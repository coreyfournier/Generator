#pragma once
#include "Pin.cpp"

class IPinChangeListner
{
    public:
    virtual void PinChange(Pin& pin) = 0;    
};