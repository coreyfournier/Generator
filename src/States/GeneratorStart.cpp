#include "IState.h"
#include "Orchestration.cpp"
using namespace std;

namespace States
{
    class GeneratorStart:IState
    {
        private:
        Orchestration _context;

        public:
        GeneratorStart(Orchestration context) : _context(context)
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
            return "GeneratorStart";
        }
    };
}
