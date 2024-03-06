#pragma once
#include "IQueue.h"
#include "States/ChangeMessage.cpp"

namespace IO
{
    class MockQueue: public IQueue
    {
        private:
        

        public:
        MockQueue()
        {
            //this->_pinQueueChange = xQueueCreate(10, sizeof(States::ChangeMessage)); 
        }

        void QueueMessage(States::ChangeMessage& cm)
        {
            //xQueueSendToBackFromISR(this->_pinQueueChange, (void *)&cm, NULL);
        }

        States::ChangeMessage BlockAndDequeue()
        {
            struct States::ChangeMessage changeMessage;

            //xQueueReceive(this->_pinQueueChange, &( changeMessage ), portMAX_DELAY);

            return changeMessage;
        }

    };
    
}