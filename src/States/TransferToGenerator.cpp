#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "config.h"


using namespace std;

namespace States
{
    class TransferToGenerator: public IState
    {
        private:
        IContext* _context;
        int _delayBetweenStarts;

        public:
        TransferToGenerator(IContext* context) : 
            _context(context)
        {
            
        }

        void DoAction()
        {
            auto* utility = this->_context->GetUtility();
            auto* generator = this->_context->GetGenerator();
            auto* transferSwitch = this->_context->GetTransferSwitch();

            if(utility->IsOn())
                this->_context->StateChange(Event::Utility_On);
            else
            {
                if(generator->IsOn())
                {
                    //No other state changes, this is the last state
                    transferSwitch->Toggle(true);
                }
                else // It's not on for some reason, so switch to that state
                    this->_context->StateChange(Event::Generator_On);                
            }
        }

        string GetName()
        {
            return "Transfer to generator";
        }
    };
}
