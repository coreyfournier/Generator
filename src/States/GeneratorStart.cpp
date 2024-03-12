#pragma once
#include "States/IState.h"
#include "States/IContext.h"
#include "config.h"

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
        
        int _delayBetweenStarts;

        public:
        GeneratorStart(IContext* context, 
            const int timeToWaitForStart = DefaultGeneratorTimeToWaitForStart, 
            const int maxTimesToStart = DefaultGeneratorTimesToStart,             
            const int delayBetweenStarts = DefaultDelayBetweenGeneratorStarts) : 
            _context(context), 
            _timeToWaitForStart(timeToWaitForStart),
            _maxTimesToStart(maxTimesToStart),            
            _delayBetweenStarts(delayBetweenStarts)
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
                            //Reset it was sucessful
                            this->_totalStartTries = 0;

                            this->_context->StateChange(Event::Generator_On);                          
                            
                            //Make sure we exit
                            return;
                        }
                        else //Failed to start. It will try again
                        {
                            this->_context->StateChange(Event::Start_Failure);
                            //Delay between starts
                            this->_context->Delay(this->_delayBetweenStarts);
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
