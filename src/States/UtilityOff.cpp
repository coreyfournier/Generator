#pragma once
#include "States/IState.h"
#include "States/Orchestration.cpp"
using namespace std;

namespace States
{
    class UtilityOff:IState
    {
        private:
        Orchestration _context;

        public:
        UtilityOff(Orchestration context) : _context(context)
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
            return "UtilityOff";
        }
    };
}
