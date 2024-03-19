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

        T* GetBuffer()
        {
            T arr[this->_buffer.size()];
            
            //Front of the array
            for(int i = this->_startIndex - 1 ; i >= 0; i--)
                arr[i] = this->_buffer[i];
            
            //End of the array 
            for(int i = this->_startIndex; i< this->_buffer.size(); i++)
                arr[i] = this->_buffer[i];


            return arr;
        }

        void Add(T item)
        {
            //It's not full yet
            if(this->_buffer.size() <= this->_maxSize)
            {
                this->_buffer.push_back(item);
            }
            else
            {

                this->_startIndex ++;               

                if(this->_startIndex > this->_maxSize)
                    this->_startIndex = 0;

                this->_buffer[this->_startIndex] = item;
            }
            
        }

    };
    
} // namespace IO

