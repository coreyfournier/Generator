#ifndef PIO_UNIT_TESTING

#pragma once
#include "IQueue.h"
#include "Arduino.h"
#include "States/ChangeMessage.cpp"

namespace IO
{
    template<typename T>
    class RtosQueue: public IQueue<T>
    {
        private:
        QueueHandle_t _pinQueueChange;

        public:
        RtosQueue()
        {
            this->_pinQueueChange = xQueueCreate(10, sizeof(T)); 
        }

        void QueueMessage(T* cm)
        {
            xQueueSendToBackFromISR(this->_pinQueueChange, (void *)cm, NULL);
        }

        T* BlockAndDequeue()
        {
            T* changeMessage;

            xQueueReceive(this->_pinQueueChange, changeMessage, portMAX_DELAY);

            return changeMessage;
        }

    };
    
} // namespace IO

#endif