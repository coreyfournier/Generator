#pragma once
#include "States/IState.h"
#include "States/IContext.h"
using namespace std;

namespace States
{
    class UtilityOff: public IState
    {
       private:
        IContext* _context;

        public:
        UtilityOff(IContext* context) : _context(context)
        {

        }

        void DoAction()
        {
            //this->_context.
        }

        void WaitDone()
        {
            
        }

        string GetName()
        {
            return "Utility Off";
        }
    };
}
