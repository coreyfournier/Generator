#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "config.h"
using namespace std;

namespace States
{
    class Disabled: public IState
    {
        private:
        IContext* _context;
        
        public:
        Disabled(IContext* context) : _context(context)
        {

        }

        void DoAction()
        {            
           //Don't do anything all actions are disabled.
        }

        string GetName()
        {
            return "Disabled";
        }
    };
}
