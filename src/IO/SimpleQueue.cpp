#pragma once
#include "IQueue.h"
#include "States/ChangeMessage.cpp"
#include <mutex>
#include <condition_variable>
#include <deque>

namespace IO
{
    template<typename T>
    class SimpleQueue: public IQueue<T>
    {
        private:
        std::mutex              d_mutex;
        std::condition_variable d_condition;
        std::deque<States::ChangeMessage> d_queue;

        public:

        void QueueMessage(T* cm)
        {
            //xQueueSendToBackFromISR(this->_pinQueueChange, (void *)&cm, NULL);
            // {
            //     std::unique_lock<std::mutex> lock(this->d_mutex);
            //     d_queue.push_front(cm);
            // }
            // this->d_condition.notify_one();
        }

        T* BlockAndDequeue()
        {
            /*
            struct States::ChangeMessage changeMessage;

            xQueueReceive(this->_pinQueueChange, &( changeMessage ), portMAX_DELAY);

            return changeMessage;
            */
            
            std::unique_lock<std::mutex> lock(this->d_mutex);

            this->d_condition.wait(lock, [=]{ 
                return !this->d_queue.empty(); 
            });

            T* rc(std::move(this->d_queue.back()));
            this->d_queue.pop_back();

            return rc;         
        }

    };
    
} // namespace IO

