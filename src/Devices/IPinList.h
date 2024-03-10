#pragma once

#include <vector>
#include "IO/Pin.cpp"

namespace Devices
{
    class IPinList
    {
        public:
        virtual std::vector<IO::Pin> GetPins() = 0;
    };
}