#ifndef PIO_UNIT_TESTING
#pragma once
#include <ArduinoJson.h>
#include <WebServer.h>
#include "IController.h"
#include "States/Orchestration.cpp"
#include "config.h"
using namespace std;


namespace SimpleWeb
{
    class DataController: public IController
    {
        private:
        States::Orchestration* _view;
        WebServer* _server = nullptr;

        void PostData()
        {
            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, _server->arg("plain"));

            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());

                doc.clear();
                doc["success"] = false;
                doc["message"] = error.c_str();
                SendJson(*_server, 500, doc);
                return;
            }

            int gpio = doc["gpio"].as<int>();
            Pin* foundPin = this->_view->FindByGpio(gpio);

            if(foundPin == nullptr)
            {
                doc["success"] = false;
                doc["message"] = "Pin not found";
            }
            else
            {
                if(doc["state"].as<bool>())
                {
                    this->_view->DigitalWrite(*foundPin, true);

                    if(foundPin->role == PinRole::Start || foundPin->role == PinRole::Stop)
                    {
                        vTaskDelay(1000);
                        this->_view->DigitalWrite(*foundPin, false);
                    }
                }
                else
                {
                    this->_view->DigitalWrite(*foundPin, false);
                }

                doc["state"] = foundPin->state;
                doc["name"] = foundPin->name;
                doc["isReadOnly"] = foundPin->isReadOnly;

                foundPin->state = doc["state"].as<bool>();
            }

            SendJson(*_server, 200, doc);
        }

        void GetData()
        {
            StaticJsonDocument<1400> doc;

            for(int i=0; i< this->_view->PinCount(); i++)
            {
                Pin pin = this->_view->GetPin(i);
                pin.state = this->_view->DigitalRead(pin);
                doc["pins"][i]["gpio"] = pin.gpio;
                doc["pins"][i]["state"] = pin.state;
                doc["pins"][i]["name"] = pin.name;
                doc["pins"][i]["isReadOnly"] = pin.isReadOnly;
            }

            SendJson(*_server, 200, doc);
        }

        public:
        DataController(States::Orchestration* view): _view(view)
        {
        }

        void Register(WebServer& server)
        {
            _server = &server;
            server.on("/data", HTTP_GET, [this]() { GetData(); });
            server.on("/data", HTTP_POST, [this]() { PostData(); });
        }
    };

}
#endif
