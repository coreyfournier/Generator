#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "config.h"

namespace States
{
    class GeneratorStop: public IState
    {
        private:
        IContext* _context;

        public:
        GeneratorStop(IContext* context) : _context(context)
        {

        }

        void DoAction()
        {
            auto* utility = this->_context->GetUtility();
            auto* generator = this->_context->GetGenerator();
            auto* transferSwitch = this->_context->GetTransferSwitch();

            if(generator->IsOn())
            {
                this->_context->StateChange(Event::Generator_Cooling_Down);
                this->_context->Delay(DefaultGeneratorCoolDownTime);
                generator->Stop();
                int attemptsToStop = 0;
                
                this->_context->StateChange(Event::Generator_Stopping);

                do
                {
                    this->_context->Delay(DefaultGeneratorTimeToWaitToStop);
                    attemptsToStop++;
                } while (generator->IsOn() && MaxAttemptsToStopGenerator < attemptsToStop);
                
                if(generator->IsOn())
                {
                    this->_context->StateChange(Event::Generator_Stop_Failed);
                }
            }

        }
      

        string GetName()
        {
            return "Generator stop";
        }
    };
}
