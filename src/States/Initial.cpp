#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "IO/Pin.cpp"
#include <stdexcept>

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
              
            this->_context->GetSerialIO()->Println("Doing action");
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
