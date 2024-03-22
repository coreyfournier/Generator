#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "config.h"


using namespace std;

namespace States
{
    class Initalize: public IState
    {
        private:
        IContext* _context;

        public:
        Initalize(IContext* context) : 
            _context(context)
        {
            
        }

        void DoAction()
        {
            auto* utility = this->_context->GetUtility();
            auto* generator = this->_context->GetGenerator();
            auto* transferSwitch = this->_context->GetTransferSwitch();

            //It's not on, so trigger the utility off state
            if(!utility->IsOn())
                this->_context->StateChange(Event::Utility_Off);
            else
                this->_context->StateChange(Event::Idle);
        }

        string GetName()
        {
            return "Initalize";
        }
    };
}