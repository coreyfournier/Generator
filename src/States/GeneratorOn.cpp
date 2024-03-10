#include "IState.h"
#include "Orchestration.cpp"
#include "Devices/PowerDevice.cpp"
#include "Devices/StartableDevice.cpp"
#include "Devices/TransferSwitch.cpp"
using namespace std;

namespace States
{
    class GeneratorOn:IState
    {
        private:
        Orchestration _context;

        public:
        GeneratorOn(Orchestration& context) : _context(context)
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
