#pragma once
#include <stdint.h>
#include "VoltageDetector.cpp"


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
    VoltageDetector *_utilitySense;
    VoltageDetector *_generatorSense;
    IEvent *_listner;


    public:
    Orchestration(
        uint16_t (*func)(uint8_t),
        uint8_t &utilitySenseGpio,
        uint8_t &generatorSenseGpio,
        IEvent *listner
    ) 
    {
        _utilitySense = new VoltageDetector(func, utilitySenseGpio);
        _generatorSense = new VoltageDetector(func, generatorSenseGpio);
        _listner = listner;
    }

    void SenseChanges()
    {        
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
    }
};