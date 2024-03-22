#ifndef PIO_UNIT_TESTING
#pragma once
#include <ArduinoJson.h>
#include "Router.h"
#include "IController.h"
#include "States/Orchestration.cpp"
#include "States/IEvent.cpp"
#include "IO/ISerial.h"
using namespace std;


namespace SimpleWeb
{
    class StateController: public IController
    {
        private:
        States::Orchestration* _view;

        public:
        StateController(States::Orchestration* view): _view(view)
        {
        }

        bool Handler(WiFiClient& client, const String& header)
        {
            if(header.indexOf("POST /state HTTP/1.1") >= 0)
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
                    States::Event event = (States::Event)doc["eventId"].as<int>();                         
                    if(event == States::Event::Idle || event == States::Event::Disable)
                    {
                        this->_view->StateChange(event);
                        doc["success"] = true;
                        doc["message"] = IO::string_format("Changed to state %s", States::IEvent::ToName(event));   
                        
                        client.println("HTTP/1.1 200 OK");
                    }
                    else
                    {
                        client.println("HTTP/1.1 500 Internal Server Error");
                        doc["success"] = false;
                        doc["message"] = "State isn't allowed. Only Idle or Disabled";      
                    }                   
                    
                    client.println("Content-type:text/json");
                    client.println("Connection: close");
                    client.println();
                }

                serializeJson(doc, client);  

                return true;            
            }
            else if(header.indexOf("GET /state HTTP/1.1") >= 0)
            {   
                StaticJsonDocument<1400> doc;      
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/json");
                client.println("Connection: close");
                client.println(); 
                
                Serial.printf("data...");
                
                doc["current"] = this->_view->GetStateName();         
                auto lastEvents = this->_view->GetLastEvents();
                int i=0;
                for (auto e = lastEvents.begin(); e != lastEvents.end(); ++e)
                {
                    doc["lastEvents"][i] = States::IEvent::ToName(*e);
                    i++;
                }              
                           
                serializeJson(doc, client);

                return true;
            }

            return false;
        }
    };

}
#endif