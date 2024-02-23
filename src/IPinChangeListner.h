#pragma once
#include "Pin.cpp"

class IPinChangeListner
{
    public:
    virtual void ChangeListner(Pin& pin) = 0;    
};