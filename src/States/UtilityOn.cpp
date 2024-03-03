#pragma once
#include "States/IState.h"
#include "States/IContext.h"
using namespace std;

namespace States
{
    class UtilityOn: public IState
    {
        private:
        IContext _context;

        public:
        UtilityOn(IContext& context) : _context(context)
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
