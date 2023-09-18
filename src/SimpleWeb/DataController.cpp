#pragma once
#include <ArduinoJson.h>
#include "Router.h"
#include "GeneratorView.cpp"
#include "IController.h"
using namespace std;


namespace SimpleWeb
{
    class DataController: public IController
    {
        GeneratorView _view;

        public:
        DataController(GeneratorView& view): _view(view)
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
                    if(doc["state"].as<bool>())
                    digitalWrite(doc["gpio"].as<int>(), HIGH);
                    else
                    digitalWrite(doc["gpio"].as<int>(), LOW);

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
                StaticJsonDocument<200> doc;      
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/json");
                client.println("Connection: close");
                client.println(); 
                
                Serial.printf("data...");
                Serial.printf("pins=%i\n", sizeof(_view.pins));

                for(int i=0; i< _view.pins.size(); i++)
                {
                    Pin pin = _view.pins[i];
                    Serial.printf("digitalRead...\n");
                    pin.state = (digitalRead(pin.gpio) ==  HIGH);
                    doc["pins"][i]["gpio"] = pin.gpio;
                    doc["pins"][i]["state"] = (digitalRead(pin.gpio) ==  HIGH);
                    doc["pins"][i]["name"] = pin.name;                
                }                            
                serializeJson(doc, client);

                return true;
            }

            return false;
        }
    };

}