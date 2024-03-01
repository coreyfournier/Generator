#include "IState.h"
#include "Orchestration.cpp"
using namespace std;

namespace States
{
    class GeneratorOn:IState
    {
        private:
        Orchestration _context;

        public:
        GeneratorOn(Orchestration context) : _context(context)
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
            return "GeneratorOn";
        }
    };
}
