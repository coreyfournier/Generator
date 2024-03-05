#pragma once
#include "States/ChangeMessage.cpp"

namespace IO
{
    class IQueue
    {
        public:
        virtual void QueueMessage(States::ChangeMessage& cm) = 0;

        virtual States::ChangeMessage BlockAndDequeue() = 0;
        
    };
    
} 

