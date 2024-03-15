#pragma once
#include <string>
#include "IO/Pin.cpp"

using namespace std;

namespace States
{
    /// @brief All the event phases. The order matters here as they are step by step progression, it also matches the ToName function
    enum class Event:int { 
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
        Generator_Stop = 14,
        Generator_Cooling_Down = 15,         
        Generator_Stopping = 16,
        Generator_Stop_Failed = 17,
        Generator_Off = 18, 

        //Any other events
        Start_Failure = 19,
        Idle = 20,
        Disable = 21,
        PinChange_Read_Value = 22,
        PinChange_No_Read_Value = 23
    };

    class IEvent
    {
        private:
    
        public:
        virtual void Change(Event e) = 0;

        static string ToName(Event e)
        {
            static string _enumStrings[24] = { 

                "Initalize",
                //Startup phases
                "Utility Off",
                "UtilityOffWait",
                "UtilityOffWaitDone",   
                         
                "GeneratorStart",
                "GeneratorStarting",
                "GeneratorOn",
                "GeneratorWarmUp",
                "GeneratorWarmUpDone",
                "TransferToGenerator",

                //Shut down phases
                "UtilityOn",
                "UtilityOnWait",
                "UtilityOnWaitDone",
                "TransferToUtility",
                "GeneratorStop",
                "GeneratorCoolingDown", 
                "GeneratorStopping",
                "GeneratorStopFailed",
                "GeneratorOff",

                //Error states
                "StartFailure",
                "Idle",
                "Disable",
                "PinChangeRead",
                "PinChangeNoRead"
            };
            
            return _enumStrings[(int)e];
        }
    };
}