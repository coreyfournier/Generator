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
        /// @brief 
        /// @param queueSize How many items the queue can hold
        RtosQueue(int queueSize = 10)
        {
            this->_pinQueueChange = xQueueCreate(queueSize, sizeof(T*)); 
        }

        void QueueMessage(T* cm)
        {
            xQueueSendToBackFromISR(this->_pinQueueChange, &cm, NULL);
        }

        T* BlockAndDequeue()
        {
            T* changeMessage;

            xQueueReceive(this->_pinQueueChange, &(changeMessage), portMAX_DELAY);

            return changeMessage;
        }

    };
    
} // namespace IO

#endif