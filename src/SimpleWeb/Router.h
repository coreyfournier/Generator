#pragma once
#include <WiFi.h>
#include <vector>
#include "IController.h"
using namespace std;


namespace SimpleWeb
{
    class Router
    {
        private:
        WiFiServer &_server;
        vector<IController*>  _controllers;
        // Define timeout time in milliseconds (example: 2000ms = 2s)
        long timeoutTime = 2000;
        // Current time
        unsigned long currentTime = millis();
        // Previous time
        unsigned long previousTime = 0; 


        public:

        Router(WiFiServer &server) : _server(server){
            //_server = server;
        }

        void AddController(IController*  controller)
        {            
            _controllers.push_back(controller);
        }


        /*Checks for new clients and handles them*/
        void Check();
    };
}