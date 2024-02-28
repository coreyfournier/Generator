#pragma once
#include <stdio.h>
#include <ArduinoJson.h>
#include "Pin.cpp"
#include "PowerState.h"
#include "IPinChangeListner.h"
using namespace std;

namespace IO {

    PowerState::PowerState(){
        AMessage xMessage;
        tsqueue = xQueueCreate(4, sizeof(xMessage));        
    }
    
    void PowerState::RegisterListner(IPinChangeListner* listner) 
    {
        PowerState::listner = listner;
    } 

    void PowerState::StateChangeTaskHandler(void *pvParameters)
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
                //Update the state
                xRxedStructure.pin->state = (state == HIGH);
                printf("%s interrupt fired at %dms by pin %i value=%i\r\n",xRxedStructure.pin->name.c_str(), xRxedStructure.time, xRxedStructure.pin->gpio, state);

                //Notify of the change
                if(listner != nullptr)
                    listner->PinChange(*xRxedStructure.pin);

                last = xRxedStructure.time;
            }
        }
    }

    // Initialize static member of class Box
    int PowerState::objectCount = 0;
    IPinChangeListner* PowerState::listner = nullptr;
}