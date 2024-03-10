#pragma once
#include "States/IState.h"
#include "States/IContext.h"
using namespace std;

namespace States
{
    class UtilityOff: public IState
    {
        private:
        IContext* _context;
        
        public:
        UtilityOff(IContext* context) : _context(context)
        {

        }

        void DoAction()
        {            
            this->_context->GetSerialIO()->Println("Utility off do action");

            this->_context->StateChange(Event::Utility_Off_Wait);
            this->_context->Delay(2000);
            
            this->_context->StateChange(Event::Utility_Off_Wait_Done);
            
            auto utility = this->_context->GetUtility();
            if(utility->IsOn())
                this->_context->StateChange(Event::Utility_On);
            else
            {
                auto generator = this->_context->GetGenerator();
                if(generator->IsOn())
                    this->_context->StateChange(Event::Generator_On);
                else
                    this->_context->StateChange(Event::Generator_Start);
            }    
        }

        string GetName()
        {
            return "Utility Off";
        }
    };
}
