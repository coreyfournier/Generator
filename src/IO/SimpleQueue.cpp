#pragma once
#include "IQueue.h"
#include <stdexcept>
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
            if(this->_queue.empty())
                throw std::runtime_error("Queue is empty");

            auto t = this->_queue.front();
            this->_queue.pop();
            return t;
        }

        bool IsEmpty()
        {
            return this->_queue.empty();
        }

    };

} // namespace IO

