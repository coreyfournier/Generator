#ifndef PIO_UNIT_TESTING

#pragma once
#include "ISerial.h"
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
        IO::ISerial* _serial;

        public:
        /// @brief 
        /// @param serial serial writer for debugging
        /// @param queueSize How many items the queue can hold
        RtosQueue(IO::ISerial* serial, int queueSize = 10) : _serial(serial)
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