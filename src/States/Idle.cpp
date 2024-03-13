#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "config.h"


using namespace std;

namespace States
{
    class Idle: public IState
    {
        private:
        IContext* _context;

        public:
        Idle(IContext* context) : 
            _context(context)
        {
            
        }

        void DoAction()
        {
            auto* utility = this->_context->GetUtility();
            auto* generator = this->_context->GetGenerator();
            auto* transferSwitch = this->_context->GetTransferSwitch();

        }

        string GetName()
        {
            return "Idle";
        }
    };
}