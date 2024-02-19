#pragma once

#include <stdio.h>
#include <ArduinoJson.h>
#include "GeneratorView.cpp"
using namespace std;

struct AMessage
{
    AMessage(){}
    uint32_t time;
    Pin* pin;
};

class PowerState
{   
    private:
    int GeneratorOnSenseGPIO;    

    public:    
    QueueHandle_t tsqueue;

    PowerState(){
        AMessage xMessage;
        PowerState::tsqueue = xQueueCreate(4, sizeof(xMessage));        
    }
   
    static void StateChangeTaskHandler(void *pvParameters)
    {
        printf("Waiting for button press interrupt on gpio...\r\n");
        QueueHandle_t *queueParam = (QueueHandle_t *)pvParameters;
     
        uint32_t last = 0;
        while(1) {
            struct AMessage xRxedStructure, *pxRxedPointer;

            xQueueReceive(*queueParam, &( xRxedStructure ), portMAX_DELAY);

            xRxedStructure.time *= portTICK_PERIOD_MS;

            if(last < xRxedStructure.time - 200) 
            {
                //xRxedStructure->pin.gpio
                int state = digitalRead(xRxedStructure.pin->gpio);
                printf("%s interrupt fired at %dms by pin %i value=%i\r\n",xRxedStructure.pin->name.c_str(), xRxedStructure.time, xRxedStructure.pin->gpio, state);
                last = xRxedStructure.time;
            }
        }
    }
};