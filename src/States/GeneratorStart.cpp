#pragma once
#include "States/IState.h"
#include "States/IContext.h"

using namespace std;

namespace States
{
    class GeneratorStart: public IState
    {
        private:
        IContext* _context;
        int _timeToWaitForStart;
        int _maxTimesToStart;
        int _totalStartTries = 0;

        public:
        GeneratorStart(IContext* context, const int timeToWaitForStart, const int maxTimesToStart = 3) : 
            _context(context), 
            _timeToWaitForStart(timeToWaitForStart),
            _maxTimesToStart(maxTimesToStart)
        {
            
        }

        void DoAction()
        {
            auto* utility = this->_context->GetUtility();
            auto* generator = this->_context->GetGenerator();

            if(utility->IsOn())
                this->_context->StateChange(Event::Utility_On);
            else
            {
                if(generator->IsOn())
                    this->_context->StateChange(Event::Generator_On);
                else
                {                    
                    do
                    {
                        //Start the generator  
                        generator->Start();

                        this->_context->StateChange(Event::Generator_Starting);
                        this->_context->Delay(this->_timeToWaitForStart);

                        if(generator->IsOn())    
                        {
                            this->_context->StateChange(Event::Generator_Warm_Up);
                            //Reset it was sucessful
                            this->_totalStartTries = 0;
                            return;
                        }
                        else //Failed to start. It will try again
                        {
                            this->_context->StateChange(Event::Start_Failure);
                            //Delay between starts
                            this->_context->Delay(2000);
                            this->_totalStartTries++;
                        }

                        this->_context->GetSerialIO()->Println(IO::string_format("Failed to start %i times\n", this->_totalStartTries));

                    } while (this->_totalStartTries < this->_maxTimesToStart);                    
                }
            }
        }

        string GetName()
        {
            return "Generator Start";
        }
    };
}
