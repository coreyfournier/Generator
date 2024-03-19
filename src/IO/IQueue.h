#pragma once
#include "States/ChangeMessage.cpp"

namespace IO
{
    template<typename T>
    class IQueue
    {
        public:
        virtual void QueueMessage(T* cm) = 0;

        virtual T* BlockAndDequeue() = 0;
        
    };
    
} 

