#pragma once
#include <stdint.h>
#include "PowerState.cpp"
#include "GeneratorView.cpp"
#include "IO/IBoardIo.h"

using namespace std;

enum Event { 
    UtilityOn, 
    UtilityOff, 
    GeneratorOn, 
    GeneratorOff, 
    GeneratorStarting, 
    GeneratorCoolingDown, 
    GeneratoredStopping
};

class IEvent
{
    public:
    virtual void Change(Event e) = 0;
};

class Orchestration
{
    private:
    PowerState *_powerState;
    IEvent *_listner;
    IO::IBoardIO  *_board;

    void ChangeListner(Pin* pin)
    {

        printf("Change found %s", pin->name.c_str());
    }
    
    public:
    Orchestration(
        PowerState* powerState,
        IEvent *listner,
        IO::IBoardIO *board
    ) 
    {
        this->_powerState = powerState;
        this->_listner = listner;
        this->_board = board;
        //this->_powerState->RegisterListner(&Orchestration::ChangeListner);
    }
    
    void SenseChanges()
    {      
        /*
        if(_utilitySense->IsChanged())
        {
            if(_utilitySense->IsOn())
                _listner->Change(Event::UtilityOn);
            else
                _listner->Change(Event::UtilityOff);
        }

        if(_generatorSense->IsChanged())
        {
            if(_generatorSense->IsOn())
                _listner->Change(Event::GeneratorOn);
            else
                _listner->Change(Event::GeneratorOff);
        }    
        */             
    }
};