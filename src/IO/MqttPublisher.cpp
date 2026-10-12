#ifndef PIO_UNIT_TESTING
#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "config.h"
#include "IO/ISerial.h"
#include "IO/IBoardIo.h"
#include "IO/IPinStateListner.h"
#include "IO/PinTopic.h"
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

    /// @brief Copied by value into the RTOS queue so nothing is allocated on the pin or state task.
    struct PinChangeNotice
    {
        PinRole role;
        int gpio;
        bool state;
    };

    /// @brief Publishes state machine and pin changes to an MQTT broker.
    /// When a host is configured (ENV_MQTT_HOST) it is used directly on MqttDefaultPort and mDNS is skipped.
    /// Otherwise the broker is found automatically with mDNS: the _mqtt._tcp service first, then the MqttFallbackHosts names.
    /// Discovery is repeated whenever the connection drops so the broker can move without a reflash.
    /// mDNS must already be started (MDNS.begin) before Run is called.
    ///
    /// Topics (<root> = <MqttBaseTopic>/<DeviceHostName>):
    ///   <root>/status  "online" / "offline" (retained, offline is the last will)
    ///   <root>/state   current event name, e.g. "Generator On" (retained)
    ///   <root>/event   {"state":"Generator On","event":6,"time":1234} for every event (not retained)
    ///   <root>/pin/<key>  "ON" / "OFF" for every pin (retained), keys from PinTopicKey: utility_l1,
    ///                     generator_l1, transfer, start, stop... Published on each change, for every pin
    ///                     on (re)connect, and when the periodic re-read finds an input that differs.
    class MqttPublisher : public States::IStateChangeListner, public IPinStateListner
    {
        private:
        QueueHandle_t _queue;
        QueueHandle_t _pinQueue;
        IBoardIO* _board;
        std::vector<Pin*> _pins;
        /// @brief Last payload sent per pin topic key. Only used on the MQTT task.
        std::map<string, bool> _publishedPins;
        uint32_t _lastPinResync = 0;
        string _pinTopicRoot;
        WiFiClient _wifiClient;
        PubSubClient _client;
        ISerial* _serial;
        const char* _host;
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
            if(_host != nullptr && _host[0] != '\0')
            {
                //Resolved by PubSubClient on each connect (IP or DNS name). A .local name would go back through mDNS.
                _client.setServer(_host, MqttDefaultPort);
                Log(string_format("MQTT: using configured broker %s:%i", _host, MqttDefaultPort));
            }
            else
            {
                IPAddress ip;
                uint16_t port;

                if(!Discover(ip, port))
                    return false;

                _client.setServer(ip, port);
            }

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

            //Pin changes queued while disconnected are older than what the pins read now.
            xQueueReset(_pinQueue);
            PublishAllPins(true);

            return true;
        }

        void PublishPin(PinRole role, int gpio, bool state, bool force)
        {
            string key = PinTopicKey(role, gpio);
            auto last = _publishedPins.find(key);

            if(!force && last != _publishedPins.end() && last->second == state)
                return;

            _publishedPins[key] = state;
            _client.publish((_pinTopicRoot + key).c_str(), PinPayload(state), true);
        }

        /// @brief Publishes the current state of every pin. Inputs are read from the board; outputs
        /// use the state recorded by the last write.
        /// @param force publish even if unchanged since the last publish
        void PublishAllPins(bool force)
        {
            for(size_t i = 0; i < _pins.size(); i++)
            {
                Pin* pin = _pins[i];
                bool state = pin->isReadOnly ? _board->DigitalRead(*pin) : pin->state;
                PublishPin(pin->role, pin->gpio, state, force);
            }

            _lastPinResync = millis();
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
        /// @param host broker IP or DNS name, empty or nullptr to discover it with mDNS. Must outlive this object.
        /// @param user broker user name, empty or nullptr for anonymous
        /// @param password broker password
        /// @param board used to read the input pins
        /// @param pins every pin to publish. Must outlive this object.
        MqttPublisher(ISerial* serial, const char* host, const char* user, const char* password,
            IBoardIO* board, std::vector<Pin*> pins) :
            _board(board),
            _pins(pins),
            _client(_wifiClient),
            _serial(serial),
            _host(host),
            _user(user),
            _password(password)
        {
            _queue = xQueueCreate(MqttQueueSize, sizeof(StateChangeNotice));
            _pinQueue = xQueueCreate(MqttQueueSize, sizeof(PinChangeNotice));

            uint8_t mac[6];
            WiFi.macAddress(mac);
            _clientId = string_format("%s-%02x%02x%02x", DeviceHostName, mac[3], mac[4], mac[5]);

            string root = string_format("%s/%s", MqttBaseTopic, DeviceHostName);
            _statusTopic = root + "/status";
            _stateTopic = root + "/state";
            _eventTopic = root + "/event";
            _pinTopicRoot = root + "/pin/";
        }

        /// @brief Called on the state machine task. Never blocks; drops the notice if the queue is full.
        void OnStateChanged(States::Event event, uint32_t time)
        {
            StateChangeNotice notice = { event, time };
            xQueueSend(_queue, &notice, 0);
        }

        /// @brief Called on the pin, state or web task. Never blocks; drops the notice if the queue is full
        /// (the periodic re-read catches inputs up; outputs are republished on reconnect).
        void OnPinChanged(const Pin& pin)
        {
            PinChangeNotice notice = { pin.role, pin.gpio, pin.state };
            xQueueSend(_pinQueue, &notice, 0);
        }

        /// @brief Task loop: keeps the broker connection alive and publishes queued state changes.
        void Run()
        {
            //The settings are compiled in, so this shows what the build saw in ENV_MQTT_HOST / USER / PW.
            //The password is never logged, only whether it is set.
            bool hasHost = _host != nullptr && _host[0] != '\0';
            bool hasUser = _user != nullptr && _user[0] != '\0';
            size_t passwordLength = _password == nullptr ? 0 : strlen(_password);
            Log(string_format("MQTT: settings host=%s user=%s password=%s",
                hasHost ? _host : "(not set, using mDNS discovery)",
                hasUser ? _user : "(not set, anonymous)",
                passwordLength > 0 ? string_format("(set, %u chars)", (unsigned)passwordLength).c_str() : "(not set)"));

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

                PinChangeNotice pinNotice;
                while(xQueueReceive(_pinQueue, &pinNotice, 0) == pdTRUE)
                    PublishPin(pinNotice.role, pinNotice.gpio, pinNotice.state, false);

                //Catches input changes the debounce dropped (e.g. a bounce settling inside the window).
                if(millis() - _lastPinResync >= MqttPinResyncInterval)
                    PublishAllPins(false);

                _client.loop();
            }
        }
    };
}
#endif
