#ifndef PIO_UNIT_TESTING
#pragma once
#include <ArduinoJson.h>
#include <WebServer.h>
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
        WebServer* _server = nullptr;

        void PostState()
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

            States::Event event = (States::Event)doc["eventId"].as<int>();
            //User allowed states to change it to.
            if(event == States::Event::Initalize || event == States::Event::Disabled || event == States::Event::Idle)
            {
                this->_view->StateChange(event);
                doc["success"] = true;
                doc["message"] = IO::string_format("Changed to state %s", States::IEvent::ToName(event).c_str());
            }
            else
            {
                doc["success"] = false;
                doc["message"] = "State isn't allowed. Only Idle or Disabled";
            }

            SendJson(*_server, doc["success"].as<bool>() ? 200 : 500, doc);
        }

        void GetState()
        {
            StaticJsonDocument<1400> doc;
            //States the user can change it to
            doc["disabledId"] = (int)States::Event::Disabled;
            doc["enableId"] = (int)States::Event::Initalize;
            doc["idleId"] = (int)States::Event::Idle;

            JsonObject currentState = doc.createNestedObject("current");
            currentState["name"] = this->_view->GetStateName();
            currentState["id"] = (int)this->_view->GetState();

            auto lastEvents = this->_view->GetLastEvents();
            int i=0;
            for (auto e = lastEvents.begin(); e != lastEvents.end(); ++e)
            {
                doc["lastEvents"][i] = States::IEvent::ToName(*e);
                i++;
            }

            SendJson(*_server, 200, doc);
        }

        public:
        StateController(States::Orchestration* view): _view(view)
        {
        }

        void Register(WebServer& server)
        {
            _server = &server;
            server.on("/state", HTTP_GET, [this]() { GetState(); });
            server.on("/state", HTTP_POST, [this]() { PostState(); });
        }
    };

}
#endif
