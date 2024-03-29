#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "config.h"

namespace States
{
    class TransferToUtility: public IState
    {
        private:
        IContext* _context;

        public:
        TransferToUtility(IContext* context) : _context(context)
        {

        }

        void DoAction()
        {
            auto* utility = this->_context->GetUtility();
            auto* generator = this->_context->GetGenerator();
            auto* transferSwitch = this->_context->GetTransferSwitch();

            if(utility->IsOn())
            {
                if(transferSwitch->IsOnGenerator())
                    transferSwitch->Toggle(false);

                if(generator->IsOn())
                {
                    this->_context->StateChange(Event::Generator_Stop);
                    return;
                }
            }

            this->_context->StateChange(Event::Idle);
        }      

        string GetName()
        {
            return "Transfer to utility";
        }
    };
}
