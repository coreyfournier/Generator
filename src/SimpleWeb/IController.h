#pragma once
#ifndef PIO_UNIT_TESTING
#include <WebServer.h>
#include <ArduinoJson.h>
using namespace std;

namespace SimpleWeb
{
    class IController
    {
        public:
        /*
            Adds the controller's routes to the server
        */
        virtual void Register(WebServer& server) = 0;

    };

    /// @brief Serializes the document and sends it as a JSON response.
    inline void SendJson(WebServer& server, int code, const JsonDocument& doc)
    {
        String body;
        serializeJson(doc, body);
        server.send(code, "application/json", body);
    }
}
#endif
