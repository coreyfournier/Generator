#pragma once
#include <string>
#include "Pin.cpp"
using namespace std;

/// @brief All the event phases. The order matters here as they are step by step progression, it also matches the ToName function
enum Event { 
    Initalize = 0,
    //Startup phases     
    UtilityOff = 1,
    UtilityOffWait = 2,
    UtilityOffWaitDone = 3, 
    GeneratorStart = 4,
    GeneratorStarting = 5, 
    GeneratorOn = 6, 
    GeneratorWarmUp = 7,
    GeneratorWarmUpDone = 8,
    TransferToGenerator = 9,

    //Shutdown phases
    UtilityOn = 10,
    UtilityOnWait = 11,
    UtilityOnWaitDone = 12,
    TransferToUtility = 13,
    GeneratorCoolingDown = 14, 
    GeneratoredStopping = 15,
    GeneratorOff = 16, 

    //Any other events
    StartFailure = 17
    Idle = 18
};

class IEvent
{
    private:
   
    public:
    virtual void Change(Event e) = 0;

    static string ToName(Event e)
    {
         static string _enumStrings[19] = { 
            "Initalize",
            //Startup phases
            "Utility Off",
            "UtilityOffWait",
            "UtilityOffWaitDone",            
            "GeneratorStart",
            "GeneratorStarting",
            "GeneratorOn",
            "GeneratorWarmUp",
            "GeneratorWarmUpDone"
            "TransferToGenerator",

            //Shut down phases
            "UtilityOn",
            "UtilityOnWait",
            "UtilityOnWaitDone",
            "TransferToUtility",
            "GeneratorCoolingDown", 
            "GeneratoredStopping",
            "GeneratorOff",

            //Error states
            "StartFailure",
            "Idle"
         };
        return _enumStrings[(int)e];
    }
};

struct ChangeMessage
{
    Event event;
    Pin pin;
    
    ChangeMessage(){}

    ChangeMessage(Event e, Pin& pin)
    {
        this->pin = pin;
        this->event = e;
    }
};