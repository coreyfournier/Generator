#pragma once
#include "IState.h"
#include "States/Orchestration.cpp"
using namespace std;

namespace States
{
    class UtilityOn: public IState
    {
        private:
        Orchestration _context;

        public:
        UtilityOn(Orchestration context) : _context(context)
        {

        }

        void DoAction()
        {

        }

        void WaitDone()
        {
            
        }

        string GetName()
        {
            return "Utility On";
        }
    };
}
