#pragma once
#include <string>
using namespace std;

/// @brief All the event phases. The order matters here as they are step by step progression, it also matches the ToName function
enum Event { 
    Initalize = 0,
    //Startup phases     
    UtilityOff = 1,
    UtilityOffWait = 2,
    GeneratorStart = 3,
    GeneratorStarting = 4, 
    GeneratorOn = 5, 
    GeneratorWarmUp = 6,
    TransferToGenerator = 7,

    //Shutdown phases
    UtilityOn = 8,
    UtilityOnWait = 9,
    TransferToUtility = 10,
    GeneratorCoolingDown = 11, 
    GeneratoredStopping = 12,
    GeneratorOff = 13, 

    //Any other events
    StartFailure = 14
};

class IEvent
{
    private:
   
    public:
    virtual void Change(Event e) = 0;

    static string ToName(Event e)
    {
         static string _enumStrings[15] = { 
            "Initalize",
            //Startup phases
            "Utility Off",
            "UtilityOffWait",
            "GeneratorOff",
            "GeneratorStart",
            "GeneratorStarting",
            "GeneratorOn",
            "GeneratorWarmUp",
            "TransferToGenerator",

            //Shut down phases
            "UtilityOn",
            "UtilityOnWait",
            "TransferToUtility",
            "GeneratorCoolingDown", 
            "GeneratoredStopping",

            //Error states
            "StartFailure"
         };
        return _enumStrings[(int)e];
    }
};

struct ChangeMessage
{
    Event event;
    
    ChangeMessage(){}

    ChangeMessage(Event e)
    {
        this->event = e;
    }
};