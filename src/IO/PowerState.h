#pragma once
#include <stdio.h>
#include <ArduinoJson.h>
#include "Pin.cpp"
#include "IPinChangeListner.h"
using namespace std;

namespace IO {

    struct AMessage
    {
        AMessage(){}
        uint32_t time;
        Pin* pin;
    };

    class PowerState
    {  
        private:
        static IPinChangeListner* listner;    

        public:    
        QueueHandle_t tsqueue;

        PowerState();    
        
        static int objectCount;        
        
        void RegisterListner(IPinChangeListner* listner) ;

        static void StateChangeTaskHandler(void *pvParameters);
    };
}