#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "IO/Pin.cpp"
#include <stdexcept>
//#include "Arduino.h"

using namespace std;
using namespace IO;

namespace States
{
    class Initial: public IState
    {
        private:
        IContext* _context;

        public:
        Initial(IContext* context) : _context(context)
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
