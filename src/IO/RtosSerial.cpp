#ifndef PIO_UNIT_TESTING

#pragma once
#include "ISerial.h"
#include <stdio.h>
#include "Arduino.h"
#include <memory>
#include <string>
#include <stdexcept>
#include <Syslog.h>
#include <WiFiUdp.h>

namespace IO
{   
    class RtosSerial : public ISerial
    {
        private :        
        Syslog* _syslog;
        WiFiUDP _udpClient;

        public:

        RtosSerial(): _syslog(nullptr)
        {

        }

        RtosSerial(Syslog* syslog) : _syslog(syslog)
        {            
            
        }

        void Print(const std::string& format) 
        {
            auto output = format.c_str();
            Serial.print(output);
            
            if(this->_syslog != nullptr)
            {
                try {
                    _syslog->log(LOG_INFO, output);
                }
                catch (...) {
                    Serial.println("Fail to send log");
                }
            }  
        }       

        void Println(const std::string& format)
        {
            auto output = format.c_str();                        
            Serial.println(output);

            if(this->_syslog != nullptr)
            {
                try {
                    _syslog->log(LOG_INFO, output);

                }
                catch (...) {
                    Serial.println("Fail to send log");
                }
            }           
        }
    };
}
#endif