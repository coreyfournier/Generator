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
    VoltageDetector *_utilitySenseL1;
    VoltageDetector *_utilitySenseL2;
    VoltageDetector *_generatorSense;
    IEvent *_listner;
    
    public:
    /// @brief 
    /// @param func 
    /// @param utilitySenseL1Gpio Gpio pin for sensing if L1 has power
    /// @param utilitySenseL2Gpio Gpio pin for sensing if L2 has power
    /// @param generatorSenseGpio Gpio pin for sensing if the generator is running
    /// @param listner call back for events
    Orchestration(
        uint16_t (*func)(uint8_t),
        uint8_t &utilitySenseL1Gpio,
        uint8_t &utilitySenseL2Gpio,
        uint8_t &generatorSenseGpio,
        IEvent *listner
    ) 
    {
        _utilitySenseL1 = new VoltageDetector(func, utilitySenseL1Gpio);
        _utilitySenseL2 = new VoltageDetector(func, utilitySenseL2Gpio);
        _generatorSense = new VoltageDetector(func, generatorSenseGpio);
        _listner = listner;
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