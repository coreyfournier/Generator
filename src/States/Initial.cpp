#pragma once
#include "IState.h"
#include "States/IContext.h"
//#include "States/Orchestration.cpp"
using namespace std;

namespace States
{
    class Initial: public IState
    {
        private:
        IContext _context;

        public:
        Initial(IContext& context) : _context(context)
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
