#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "config.h"

namespace States
{
    class GeneratorStop: public IState
    {
        private:
        IContext* _context;

        public:
        GeneratorStop(IContext* context) : _context(context)
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
            return "Generator stop";
        }
    };
}
