#ifndef PIO_UNIT_TESTING
#pragma once
#include <string>
#include <stdio.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "config.h"
#include "IO/ISerial.h"
#include "States/IEvent.cpp"
#include "States/IStateChangeListner.h"

using namespace std;

namespace IO
{
    /// @brief Copied by value into the RTOS queue so nothing is allocated on the state machine task.
    struct StateChangeNotice
    {
        States::Event event;
        uint32_t time;
    };

    /// @brief Publishes state changes to an MQTT broker that is found automatically with mDNS.
    /// The broker is looked up as the _mqtt._tcp service first, then by the MqttFallbackHosts names.
    /// Discovery is repeated whenever the connection drops so the broker can move without a reflash.
    /// mDNS must already be started (MDNS.begin) before Run is called.
    ///
    /// Topics (<root> = <MqttBaseTopic>/<DeviceHostName>):
    ///   <root>/status  "online" / "offline" (retained, offline is the last will)
    ///   <root>/state   current event name, e.g. "Generator On" (retained)
    ///   <root>/event   {"state":"Generator On","event":6,"time":1234} for every event (not retained)
    class MqttPublisher : public States::IStateChangeListner
    {
        private:
        QueueHandle_t _queue;
        WiFiClient _wifiClient;
        PubSubClient _client;
        ISerial* _serial;
        const char* _user;
        const char* _password;
        bool _hasLast = false;
        StateChangeNotice _last;
        string _clientId;
        string _statusTopic;
        string _stateTopic;
        string _eventTopic;

        void Log(const string& message)
        {
            if(_serial != nullptr)
                _serial->Println(message);
        }

        /// @brief Finds the broker on the local network.
        /// @return true if a broker address was found
        bool Discover(IPAddress& ip, uint16_t& port)
        {
            int found = MDNS.queryService("mqtt", "tcp");
            if(found > 0)
            {
                ip = MDNS.IP(0);
                port = MDNS.port(0);
                Log(string_format("MQTT: found broker service %s at %s:%i", MDNS.hostname(0).c_str(), ip.toString().c_str(), port));
                return true;
            }

            for(size_t i = 0; i < sizeof(MqttFallbackHosts) / sizeof(MqttFallbackHosts[0]); i++)
            {
                IPAddress hostIp = MDNS.queryHost(MqttFallbackHosts[i]);
                if((uint32_t)hostIp != 0)
                {
                    ip = hostIp;
                    port = MqttDefaultPort;
                    Log(string_format("MQTT: found broker host %s.local at %s:%i", MqttFallbackHosts[i], ip.toString().c_str(), port));
                    return true;
                }
            }

            Log("MQTT: no broker found via mDNS");
            return false;
        }

        bool Connect()
        {
            IPAddress ip;
            uint16_t port;

            if(!Discover(ip, port))
                return false;

            _client.setServer(ip, port);

            bool hasUser = _user != nullptr && _user[0] != '\0';
            bool connected = _client.connect(
                _clientId.c_str(),
                hasUser ? _user : nullptr,
                hasUser ? _password : nullptr,
                _statusTopic.c_str(), 0, true, "offline");

            if(!connected)
            {
                Log(string_format("MQTT: connect failed, state=%i", _client.state()));
                return false;
            }

            Log(string_format("MQTT: connected as %s", _clientId.c_str()));
            _client.publish(_statusTopic.c_str(), "online", true);

            //Anything missed while disconnected is summarized by the latest state.
            if(_hasLast)
                Publish(_last);

            return true;
        }

        void Publish(const StateChangeNotice& notice)
        {
            string name = States::IEvent::ToName(notice.event);
            string json = string_format("{\"state\":\"%s\",\"event\":%i,\"time\":%u}", name.c_str(), (int)notice.event, notice.time);

            _last = notice;
            _hasLast = true;

            _client.publish(_stateTopic.c_str(), name.c_str(), true);
            _client.publish(_eventTopic.c_str(), json.c_str(), false);
        }

        public:
        /// @param serial logging output, can be nullptr
        /// @param user broker user name, empty or nullptr for anonymous
        /// @param password broker password
        MqttPublisher(ISerial* serial, const char* user, const char* password) :
            _client(_wifiClient),
            _serial(serial),
            _user(user),
            _password(password)
        {
            _queue = xQueueCreate(MqttQueueSize, sizeof(StateChangeNotice));

            uint8_t mac[6];
            WiFi.macAddress(mac);
            _clientId = string_format("%s-%02x%02x%02x", DeviceHostName, mac[3], mac[4], mac[5]);

            string root = string_format("%s/%s", MqttBaseTopic, DeviceHostName);
            _statusTopic = root + "/status";
            _stateTopic = root + "/state";
            _eventTopic = root + "/event";
        }

        /// @brief Called on the state machine task. Never blocks; drops the notice if the queue is full.
        void OnStateChanged(States::Event event, uint32_t time)
        {
            StateChangeNotice notice = { event, time };
            xQueueSend(_queue, &notice, 0);
        }

        /// @brief Task loop: keeps the broker connection alive and publishes queued state changes.
        void Run()
        {
            while(true)
            {
                if(WiFi.status() != WL_CONNECTED)
                {
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    continue;
                }

                if(!_client.connected() && !Connect())
                {
                    vTaskDelay(pdMS_TO_TICKS(MqttRetryDelay));
                    continue;
                }

                StateChangeNotice notice;
                if(xQueueReceive(_queue, &notice, pdMS_TO_TICKS(100)) == pdTRUE)
                    Publish(notice);

                _client.loop();
            }
        }
    };
}
#endif
