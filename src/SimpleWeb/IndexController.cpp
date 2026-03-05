
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
        bool Handler(WiFiClient& client, const String& header)
        {
            if(header.indexOf("GET / HTTP/1.1") >= 0)
            {
                fs::FS fs = SPIFFS;
                if(SPIFFS.exists("/index.html"))
                {
                    File file = fs.open("/index.html");
                    size_t fileSize = file.size();

                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println("Connection: close");
                    client.print("Content-Length: ");
                    client.println(fileSize);
                    client.println();

                    uint8_t buf[256];
                    while(file.available())
                    {
                        size_t len = file.read(buf, sizeof(buf));
                        client.write(buf, len);
                    }
                    file.close();
                }
                else
                {
                    String body = "Can't find index.html";
                    client.println("HTTP/1.1 404 Not Found");
                    client.println("Content-type:text/plain");
                    client.println("Connection: close");
                    client.print("Content-Length: ");
                    client.println(body.length());
                    client.println();
                    client.print(body);
                    Serial.println("Can't find index.html");
                }

                return true;
            }

            return false;
        }
    };

}
#endif