#pragma once
#include <WiFi.h>
#include <WiFiUdp.h>
#include <stdio.h>
#include "Arduino.h"
#include "IO/ISerial.h"

namespace IO
{
    class WifiHelper{
        private:
        char* _password;
        char* _ssid;
        int _connectionWaitIteration = 5;

        void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
            Serial.println("Disconnected from WiFi access point");
            Serial.print("WiFi lost connection. Reason: ");
            Serial.println(info.wifi_sta_disconnected.reason);
        }

        void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
            Serial.println("Connected to AP successfully!");
        }

        public:
        WifiHelper(char* ssid, char* password) : _ssid(ssid), _password(password)
        {

        };

        wl_status_t Connect(bool block)
        {
            WiFi.disconnect(true);

            //WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
            //WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

            // Connect to Wi-Fi network with SSID and password
            Serial.print("Connecting to ");
            Serial.println(_ssid);
            WiFi.begin(_ssid, _password);
            int connectionLoops = 0;

            while ((block || connectionLoops < _connectionWaitIteration) && WiFi.status() != WL_CONNECTED) 
            {
                delay(500);
                Serial.print(".");
                connectionLoops++;
            }
            // Print local IP address and start web server
            Serial.print("\n WiFi connected.\n");
            Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
            //Serial.print(IO::string_format("IP address: %s", WiFi.localIP().toString().c_str()));

            return WiFi.status() ;
        };

        wl_status_t Status()
        {
            return WiFi.status();
        };

    };
}