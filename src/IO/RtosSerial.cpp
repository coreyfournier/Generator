#ifndef PIO_UNIT_TESTING

#pragma once
#include "ISerial.h"
#include <stdio.h>
#include "Arduino.h"
#include <memory>
#include <string>
#include <stdexcept>

namespace IO
{   
    class RtosSerial : public ISerial
    {
        public:

        void Print(const std::string& format) 
        {
            Serial.print(format.c_str());
        }       

        void Println(const std::string& format)
        {
            Serial.println(format.c_str());
        }

    };
}
#endif