#pragma once
#include <vector>

namespace IO
{
    /// @brief Creates a list of a fixed size always keeping the newest first and the oldest last. This is NOT thread safe!!!!
    /// @tparam T 
    template<typename T>
    class RingBuffer
    {
        private:
        std::vector<T> _buffer;
        int _startIndex = 0;
        int _maxSize;


        public:
        RingBuffer(int size = 10): _maxSize(size)
        {
            
        }

        const std::vector<T> GetBuffer()
        {
           return this->_buffer;           
        }

        void Add(T item)
        {
            this->_buffer.push_back(item);

            //It's at the max so take one off
            if(this->_buffer.size() >= this->_maxSize)
                this->_buffer.pop_back();    
        }

    };
    
} // namespace IO

