#pragma once
#ifndef PIO_UNIT_TESTING
#include <WiFi.h>
#include <vector>
using namespace std;

namespace SimpleWeb
{
    class IController
    {
        public:
        /*
            returns true when handled
        */
        virtual bool Handler(WiFiClient& client, const String& header) = 0;

    };
}
#endif