#pragma once
#include <list>

namespace IO
{
    /// @brief Creates a list of a fixed size always keeping the newest first and the oldest last. This is NOT thread safe!!!!
    /// @tparam T 
    template<typename T>
    class RingBuffer
    {
        private:
        std::list<T> _buffer;
        int _startIndex = 0;
        int _maxSize;


        public:
        RingBuffer(int size = 10): _maxSize(size)
        {
            
        }

        const std::list<T> GetBuffer()
        {
           return this->_buffer;           
        }

        void Add(T item)
        {
            this->_buffer.push_front(item);

            //It's at the max so take one off the front
            if(this->_buffer.size() >= this->_maxSize)
                this->_buffer.pop_back();    
        }
    };
    
} // namespace IO

