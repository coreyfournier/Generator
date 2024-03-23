#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "config.h"

namespace States
{
    class UtilityOn: public IState
    {
        private:
        IContext* _context;

        public:
        UtilityOn(IContext* context) : _context(context)
        {

        }

        void DoAction()
        {
            auto* utility = this->_context->GetUtility();
            auto* generator = this->_context->GetGenerator();
            auto* transferSwitch = this->_context->GetTransferSwitch();

            if(utility->IsOn())
            {
                this->_context->StateChange(Event::Utility_On_Wait);
                this->_context->Delay(DefaultDelayUtilityOnWaitTime);
                this->_context->StateChange(Event::Utility_On_Wait_Done);
                if(utility->IsOn())
                {
                    if(transferSwitch->IsOnGenerator())
                        this->_context->StateChange(Event::Transfer_To_Utility);
                    else if(generator->IsOn())
                        this->_context->StateChange(Event::Generator_Stop);
                    else
                        this->_context->StateChange(Event::Idle);
                }
                else                
                    this->_context->StateChange(Event::Utility_Off);
            }
        }      

        string GetName()
        {
            return "Utility On";
        }
    };
}
