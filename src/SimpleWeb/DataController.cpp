#pragma once
#include <ArduinoJson.h>
#include "Router.h"
#include "IController.h"
#include "States/Orchestration.cpp"
using namespace std;


namespace SimpleWeb
{
    class DataController: public IController
    {
        Orchestration _view;

        public:
        DataController(Orchestration& view): _view(view)
        {
        }

        bool Handler(WiFiClient& client, const String& header)
        {
            if(header.indexOf("POST /data HTTP/1.1") >= 0)
            {
                StaticJsonDocument<256> doc;
                String s = client.readStringUntil('\n');              
                DeserializationError error = deserializeJson(doc, s.c_str());   

                if (error) 
                {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.f_str());

                    doc["success"] = false;
                    doc["message"] = error.f_str();                

                    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                    // and a content-type so the client knows what's coming, then a blank line:
                    client.println("HTTP/1.1 500 Internal Server Error");
                    client.println("Content-type:text/json");
                    client.println("Connection: close");
                    client.println();
                }
                else
                {
                    int gpio = doc["gpio"].as<int>();                         
                    Pin* foundPin = _view.FindByGpio(gpio);

                    if(foundPin == nullptr)
                    {
                        doc["success"] = false;
                        doc["message"] = "Pin not found";    
                    }
                    else
                    {
                        if(doc["state"].as<bool>())
                        {
                            this->_view.DigitalWrite(*foundPin, true);

                            if(foundPin->role == PinRole::Start || foundPin->role == PinRole::Stop)
                            {
                                vTaskDelay(1000);
                                this->_view.DigitalWrite(*foundPin, false);
                            }
                        }
                        else
                        {
                            this->_view.DigitalWrite(*foundPin, false);                            
                        }

                        doc["state"] = foundPin->state;
                        doc["name"] = foundPin->name;
                        doc["isReadOnly"] = foundPin->isReadOnly;                

                        foundPin->state = doc["state"].as<bool>();
                    }
                   

                    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                    // and a content-type so the client knows what's coming, then a blank line:
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/json");
                    client.println("Connection: close");
                    client.println();
                }

                serializeJson(doc, client);  

                return true;            
            }
            else if(header.indexOf("GET /data HTTP/1.1") >= 0)
            {   
                StaticJsonDocument<600> doc;      
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/json");
                client.println("Connection: close");
                client.println(); 
                
                Serial.printf("data...");
                Serial.printf("pins=%i\n", _view.PinCount());

                for(int i=0; i< _view.PinCount(); i++)
                {
                    Pin pin = _view.GetPin(i);
                    Serial.printf("digitalRead... %s %i\n",pin.name.c_str(), pin.gpio);
                    pin.state = this->_view.DigitalRead(pin);
                    doc["pins"][i]["gpio"] = pin.gpio;
                    doc["pins"][i]["state"] = pin.state;
                    doc["pins"][i]["name"] = pin.name;
                    doc["pins"][i]["isReadOnly"] = pin.isReadOnly;                
                }                            
                serializeJson(doc, client);

                return true;
            }

            return false;
        }
    };

}