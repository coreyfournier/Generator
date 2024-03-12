#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "config.h"

using namespace std;

namespace States
{
    class GeneratorOn: public IState
    {
        private:
        IContext* _context;
        int _warmUpTime;

        public:
        GeneratorOn(IContext* context, const int warmUpTime = DefaultGeneratorWarmUpTime) : _context(context), _warmUpTime(warmUpTime)
        {

        }

        void DoAction()
        {
            auto* utility = this->_context->GetUtility();
            auto* generator = this->_context->GetGenerator();

            if(utility->IsOn())
            {
                this->_context->StateChange(Event::Utility_On);
            }
            else if(generator->IsOn())
            {
                this->_context->StateChange(Event::Generator_Warm_Up);
                this->_context->Delay(this->_warmUpTime);
                this->_context->StateChange(Event::Generator_Warm_Up_Done);
                this->_context->StateChange(Event::Transfer_To_Generator);
            }
            else
            {
                this->_context->StateChange(Event::Generator_Start);
            }
        }

        void WaitDone()
        {
            
        }

        string GetName()
        {
            return "Generator On";
        }
    };
}
