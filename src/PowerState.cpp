#pragma once

#include <stdio.h>
#include <ArduinoJson.h>
#include "GeneratorView.cpp"
#include "Pin.cpp"
using namespace std;

namespace{

    struct AMessage
    {
        AMessage(){}
        uint32_t time;
        Pin* pin;
    };

    //bool PowerState::thisTest = false; 

    class PowerState
    {  
        private:
        static bool thisTest;
        static void (*func)(Pin*);    

        public:    
        QueueHandle_t tsqueue;
        
        static int objectCount;

        PowerState(){
            AMessage xMessage;
            tsqueue = xQueueCreate(4, sizeof(xMessage));        
        }

        
        void RegisterListner(void (*func)(Pin*)) 
        {
            PowerState::func = func;
        } 

        static void StateChangeTaskHandler(void *pvParameters)
        {
            printf("Waiting for button press interrupt on gpio...\r\n");
            QueueHandle_t *queueParam = (QueueHandle_t *)pvParameters;

            thisTest = false;
        
            uint32_t last = 0;
            while(1) {
                struct AMessage xRxedStructure, *pxRxedPointer;

                xQueueReceive(*queueParam, &( xRxedStructure ), portMAX_DELAY);

                xRxedStructure.time *= portTICK_PERIOD_MS;

                if(last < xRxedStructure.time - 200) 
                {
                    //xRxedStructure->pin.gpio
                    int state = digitalRead(xRxedStructure.pin->gpio);
                    //Update the state
                    xRxedStructure.pin->state = (state == HIGH);
                    printf("%s interrupt fired at %dms by pin %i value=%i\r\n",xRxedStructure.pin->name.c_str(), xRxedStructure.time, xRxedStructure.pin->gpio, state);

                    //Notify of the change
                    //if(func != NULL)
                    //    func(xRxedStructure.pin);

                    last = xRxedStructure.time;
                }
            }
        }
    };

    // Initialize static member of class Box
    int PowerState::objectCount = 0;
    bool PowerState::thisTest  = false;
}