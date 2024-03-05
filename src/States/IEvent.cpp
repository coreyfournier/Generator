#pragma once
#include <string>
#include "Pin.cpp"
using namespace std;

namespace States
{
    /// @brief All the event phases. The order matters here as they are step by step progression, it also matches the ToName function
    enum Event { 
        Initalize = 0,
        //Startup phases     
        Utility_Off = 1,
        Utility_Off_Wait = 2,
        Utility_Off_Wait_Done = 3, 
        Generator_Start = 4,
        Generator_Starting = 5, 
        Generator_On = 6, 
        Generator_Warm_Up = 7,
        Generator_Warm_Up_Done = 8,
        Transfer_To_Generator = 9,

        //Shutdown phases
        Utility_On = 10,
        Utility_On_Wait = 11,
        Utility_On_Wait_Done = 12,
        Transfer_To_Utility = 13,
        Generator_Cooling_Down = 14, 
        Generatored_Stopping = 15,
        Generator_Off = 16, 

        //Any other events
        Start_Failure = 17,
        Idle = 18,
        Disable = 19
    };

    class IEvent
    {
        private:
    
        public:
        virtual void Change(Event e) = 0;

        static string ToName(Event e)
        {
            static string _enumStrings[20] = { 
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
                "Idle",
                "Disable"
            };
            return _enumStrings[(int)e];
        }
    };
}