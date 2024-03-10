#ifndef PIO_UNIT_TESTING

#pragma once
#include "IQueue.h"
#include "Arduino.h"
#include "States/ChangeMessage.cpp"

namespace IO
{
    class RtosQueue: public IQueue
    {
        private:
        QueueHandle_t _pinQueueChange;

        public:
        RtosQueue()
        {
            this->_pinQueueChange = xQueueCreate(10, sizeof(States::ChangeMessage)); 
        }

        void QueueMessage(States::ChangeMessage& cm)
        {
            xQueueSendToBackFromISR(this->_pinQueueChange, (void *)&cm, NULL);
        }

        States::ChangeMessage BlockAndDequeue()
        {
            struct States::ChangeMessage changeMessage;

            xQueueReceive(this->_pinQueueChange, &( changeMessage ), portMAX_DELAY);

            return changeMessage;
        }

    };
    
} // namespace IO

#endif