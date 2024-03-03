#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#include "IO/IBoardIo.h"
#include "IPinChangeListner.h"
#include "States/ChangeMessage.cpp"

using namespace States;

namespace Devices
{
    class Utility : IPinChangeListner
    {
        private:
        Pin* _L1;
        /// @brief Optional l2. When not set it is nullptr
        Pin* _L2;
        //Orchestration _orchestrator;

        public:
        /// @brief Constructor
        /// @param l1 
        /// @param l2 optional, nullptr if it doesn't exists
        //, Orchestration& orchestration
        Utility(Pin* l1, Pin* l2)
        //, _orchestrator(orchestration)
        {
                this->_L1 = l1;
                this->_L2 = l2;
        }

        /// @brief Checks if the utility is on or not
        /// @return 
        bool IsOn()
        {
            return this->_L1->state && (this->_L2 == nullptr || this->_L2->state);
        }

        void PinChange(Pin& pin)
        {
            if(pin.role == PinRole::UtilityOnL1 || pin.role == PinRole::UtilityOnL1)
            {
                if(pin.state)
                {
                    if(this->IsOn())
                    {
                        /*
                        New Process
                        -need to fire the utility on 
                        -need to wait
                        -then trigger the next state
                        */
                        //Need to send notification that the utility is now on
                        /*
                        this->_orchestrator.StateChange(Event::UtilityOn);
                        this->_orchestrator.StateChange(Event::UtilityOnWait);
                        
                        ChangeMessage cm = ChangeMessage(Event::UtilityOnWait, pin);
                        this->_orchestrator.QueueMessage(&cm);
                        */
                    }
                }
                /*
                //Came back after the wait
                else if(this->_orchestrator.GetState() == Event::UtilityOffWait)
                {
                    //It's back on now
                    if(this->IsOn())
                    {
                        this->_orchestrator.StateChange(Event::UtilityOn)
                    }
                    else //It's still off
                    {
                        
                    }
                }
                */
                //Utility is off. I don't care which leg it is, the house can't function on one.
                else
                {
                    /*
                    this->_orchestrator.StateChange(Event::UtilityOff);
                    this->_orchestrator.StateChange(Event::UtilityOffWait);

                    ChangeMessage cm = ChangeMessage(Event::UtilityOffWait, pin);
                    
                    this->_orchestrator.QueueMessage(&cm);
                    */
                }
            }
        }
    };
}