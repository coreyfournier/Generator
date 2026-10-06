#pragma once

#ifndef PIO_UNIT_TESTING
#include "FS.h"
#include "SPIFFS.h"
#include <WebServer.h>
#include "IController.h"

namespace SimpleWeb
{
    class IndexController : public IController
    {
        private:
        WebServer* _server = nullptr;

        void GetIndex()
        {
            if(SPIFFS.exists("/index.html"))
            {
                File file = SPIFFS.open("/index.html");
                //Sends Content-Length and streams the file in chunks
                _server->streamFile(file, "text/html");
                file.close();
            }
            else
            {
                Serial.println("Can't find index.html");
                _server->send(404, "text/plain", "Can't find index.html");
            }
        }

        public:
        void Register(WebServer& server)
        {
            _server = &server;
            server.on("/", HTTP_GET, [this]() { GetIndex(); });
        }
    };

}
#endif
