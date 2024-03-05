#pragma once
#include "ISerial.h"
#include <stdio.h>
#include <string>
#include <stdexcept>
#include <iostream> 
#include <unity.h>

namespace IO
{   
    /// @brief Prints the output to the console
    class CSerial : public ISerial
    {
        public:

        void Print(const std::string& format) 
        {
            UnityPrint(format.c_str());
        }       

        void Println(const std::string& format)
        {
            UnityPrint(format.c_str());
            UnityPrint("\n");
        }

    };
} // namespace IO
