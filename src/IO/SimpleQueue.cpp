#pragma once
#include "IQueue.h"
#include <mutex>
#include <queue>

namespace IO
{
    template<typename T>
    class SimpleQueue: public IQueue<T>
    {
        private:
        std::queue<T*> _queue;

        public:

        void QueueMessage(T* cm)
        {
            this->_queue.push(cm);
        }

        T* BlockAndDequeue()
        {
            auto t = this->_queue.front();
            this->_queue.pop();
            return t;         
        }

    };
    
} // namespace IO

