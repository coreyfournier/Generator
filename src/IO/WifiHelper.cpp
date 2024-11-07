#pragma once
#include <WiFi.h>
#include <stdio.h>
#include "Arduino.h"
#include "IO/ISerial.h"


using namespace std;

namespace IO
{
    class WifiHelper{
        private:
        char* _password;
        char* _ssid;
        ISerial* _serial;
        int _connectionWaitIteration = 5;

        public:
        WifiHelper(char* ssid, char* password, ISerial* serial) : _ssid(ssid), _password(password), _serial(serial)
        {
            if(_serial != nullptr)
            {
                WiFi.onEvent(
                    [&](WiFiEvent_t event, WiFiEventInfo_t info){ 
                        _serial->Println("Disconnected from WiFi access point");
                        _serial->Println(IO::string_format("WiFi lost connection. Reason: %d", info.wifi_sta_disconnected.reason));
                    }, 
                    WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

                WiFi.onEvent(
                    [&](WiFiEvent_t event, WiFiEventInfo_t info){ 
                        _serial->Println("Connected to AP successfully!");
                    }, 
                    WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
            }

        };

        wl_status_t Connect(bool block)
        {
            WiFi.disconnect(true);

            // Connect to Wi-Fi network with SSID and password
            Serial.printf("Connecting to %s\n",_ssid);
            WiFi.begin(_ssid, _password);
            int connectionLoops = 0;

            while ((block || connectionLoops < _connectionWaitIteration) && WiFi.status() != WL_CONNECTED) 
            {
                delay(500);
                connectionLoops++;
            }

            if(_serial != nullptr)
            {
                _serial->Println("WiFi connected.");
                _serial->Println(IO::string_format("IP address: %s", WiFi.localIP().toString().c_str()));
            }

            return WiFi.status();
        };

        wl_status_t Status()
        {
            return WiFi.status();
        };

    };
}