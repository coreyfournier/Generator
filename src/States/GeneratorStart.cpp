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
            auto utility = this->_context.GetUtility();
            auto generator = this->_context.GetGenerator();

            if(!utility->IsOn())
                this->_context.StateChange(Event::Utility_On);
            else
            {
                if(generator->IsOn())
                    this->_context.StateChange(Event::Generator_On);
                else
                {
                    //Start the generator  
                    
                }
            }
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
