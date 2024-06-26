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
            int attemptsToStop = 0;

            if(generator->IsOn())
            {
                this->_context->StateChange(Event::Generator_Cooling_Down);
                this->_context->Delay(DefaultGeneratorCoolDownTime);

                this->_context->StateChange(Event::Generator_Stopping);

                generator->Stop();                

                do
                {
                    this->_context->Delay(DefaultGeneratorTimeToWaitToStop);
                    attemptsToStop++;

                } while (generator->IsOn() && MaxAttemptsToStopGenerator < attemptsToStop);

                if(generator->IsOn())
                {
                    //If it fails to stop, don't go back to idle preventing it from starting again.
                    //Need to create a way to reset it.
                    this->_context->StateChange(Event::Generator_Stop_Failed);
                    return;
                }
                else
                    this->_context->StateChange(Event::Generator_Off);
            }

            this->_context->StateChange(Event::Idle);            
        }      

        string GetName()
        {
            return "Generator stop";
        }
    };
}
