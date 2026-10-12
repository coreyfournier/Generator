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
        /// @brief While blocking on a connection, scan and log what is visible every this many 500ms waits.
        int _scanEveryLoops = 30;

        void Log(const string& message)
        {
            if(_serial != nullptr)
                _serial->Println(message);
        }

        /// @brief Rough quality of a received signal strength, to make the log easy to read.
        static const char* SignalQuality(int rssi)
        {
            if(rssi >= -60) return "good";
            if(rssi >= -70) return "fair";
            if(rssi >= -80) return "weak";
            return "very weak";
        }

        /// @brief Scans and logs every access point broadcasting our SSID with its signal strength,
        /// or the strongest networks seen if ours is not found. Restarts the connection attempt afterwards.
        void ScanAndLog()
        {
            //A scan fails while a connection attempt is in progress.
            WiFi.disconnect();
            int count = WiFi.scanNetworks();
            int matches = 0;

            for(int i = 0; i < count; i++)
            {
                if(WiFi.SSID(i) != _ssid)
                    continue;

                matches++;
                Log(string_format("WiFi scan: %s channel=%i rssi=%i dBm (%s) bssid=%s",
                    _ssid, (int)WiFi.channel(i), (int)WiFi.RSSI(i), SignalQuality(WiFi.RSSI(i)), WiFi.BSSIDstr(i).c_str()));
            }

            if(matches == 0)
            {
                Log(string_format("WiFi scan: %s not seen, %i other network(s) visible", _ssid, count));
                //Results are sorted strongest first, so this shows how well the radio hears anything at all.
                for(int i = 0; i < count && i < 3; i++)
                    Log(string_format("WiFi scan:   %s rssi=%i dBm (%s)", WiFi.SSID(i).c_str(), (int)WiFi.RSSI(i), SignalQuality(WiFi.RSSI(i))));
            }

            WiFi.scanDelete();
            WiFi.begin(_ssid, _password);
        }

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
                        int rssi = WiFi.RSSI();
                        _serial->Println(IO::string_format("Connected to AP successfully! channel=%i rssi=%i dBm (%s)",
                            (int)info.wifi_sta_connected.channel, rssi, SignalQuality(rssi)));
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
            //Modem sleep (on by default) delays and drops incoming packets, making the web server flaky.
            //The setting is kept and reapplied by the core each time WiFi restarts.
            WiFi.setSleep(false);
            int connectionLoops = 0;

            while ((block || connectionLoops < _connectionWaitIteration) && WiFi.status() != WL_CONNECTED) 
            {
                delay(500);
                connectionLoops++;

                if(block && connectionLoops % _scanEveryLoops == 0)
                    ScanAndLog();
            }

            if(WiFi.status() == WL_CONNECTED)
            {
                int rssi = WiFi.RSSI();
                Log(string_format("WiFi connected. rssi=%i dBm (%s)", rssi, SignalQuality(rssi)));
                Log(string_format("IP address: %s", WiFi.localIP().toString().c_str()));
            }
            else
                Log("WiFi not connected yet.");

            return WiFi.status();
        };

        wl_status_t Status()
        {
            return WiFi.status();
        };

    };
}