
#pragma once

#ifndef PIO_UNIT_TESTING
#include <iostream>
#include <fstream>
#include "FS.h"
#include "SPIFFS.h"
#include <WiFi.h>
#include "IController.h"

namespace SimpleWeb
{
    class IndexController : public IController
    {
        void getIndex(WiFiClient& client)
        {
            fs::FS fs = SPIFFS;
            if(SPIFFS.exists("/index.html"))
            {
                File file =  fs.open("/index.html");

                Serial.println("- read from file:");
                while(file.available())
                {
                    client.write(file.read());
                }

                file.close();
            }
            else
            {
                client.println("Can't find index.html");
                Serial.println("Can't find index.html");
            }
                    
            // The HTTP response ends with another blank line
            client.println();
        }

        bool Handler(WiFiClient& client, const String& header)
        {
            if(header.indexOf("GET / HTTP/1.1") >= 0)
            {
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println("Connection: close");
                client.println(); 
                
                getIndex(client);  

                return true;
            }

            return false;
        }
    };

}
#endif