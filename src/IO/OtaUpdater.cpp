#ifndef PIO_UNIT_TESTING
#pragma once
#include <functional>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include "Arduino.h"
#include "IO/ISerial.h"

using namespace std;

namespace IO
{
    /// @brief Accepts firmware and SPIFFS uploads over WiFi (PlatformIO espota / Arduino IDE).
    /// Uploads are only accepted while isSafe() is true, and the reboot that applies the update
    /// is held until isSafe() is true again, so the controller never restarts while the house
    /// is running on the generator. mDNS must already be started; this only adds the OTA service to it.
    class OtaUpdater
    {
        private:
        const char* _hostName;
        const char* _password;
        ISerial* _serial;
        std::function<bool()> _isSafe;
        volatile bool _restartPending = false;
        int _lastPercent = -1;

        void Log(const string& message)
        {
            if(_serial != nullptr)
                _serial->Println(message);
        }

        public:
        /// @param hostName mDNS host name the device is reachable at
        /// @param password required; OTA is not started when empty
        /// @param serial logging output, can be nullptr
        /// @param isSafe returns true when it is safe to take an update and reboot
        OtaUpdater(const char* hostName, const char* password, ISerial* serial, std::function<bool()> isSafe) :
            _hostName(hostName),
            _password(password),
            _serial(serial),
            _isSafe(isSafe)
        {
        }

        /// @brief Starts listening for uploads. Call after WiFi and mDNS are started.
        /// @return false if OTA is disabled because no password was provided
        bool Begin()
        {
            if(_password == nullptr || _password[0] == '\0')
            {
                Log("OTA: disabled, no password provided (ENV_OTA_PW)");
                return false;
            }

            ArduinoOTA
                .setHostname(_hostName)
                .setPassword(_password)
                .setMdnsEnabled(false)
                .setRebootOnSuccess(false)
                .onStart([this]() {
                    _lastPercent = -1;
                    Log(ArduinoOTA.getCommand() == U_FLASH ? "OTA: receiving firmware" : "OTA: receiving filesystem");
                })
                .onProgress([this](unsigned int progress, unsigned int total) {
                    int percent = total == 0 ? 0 : (int)((progress * 100ULL) / total);
                    if(percent / 25 != _lastPercent / 25)
                    {
                        _lastPercent = percent;
                        Log(string_format("OTA: %i%%", percent));
                    }
                })
                .onEnd([this]() {
                    Log("OTA: update received, restarting once it is safe");
                    _restartPending = true;
                })
                .onError([this](ota_error_t error) {
                    Log(string_format("OTA: update failed, error=%i", (int)error));
                });

            ArduinoOTA.begin();
            MDNS.enableArduino(3232, true);
            Log(string_format("OTA: ready at %s.local", _hostName));
            return true;
        }

        /// @brief Task loop: handles uploads while safe and applies a received update when safe.
        void Run()
        {
            bool wasSafe = true;

            while(true)
            {
                bool safe = _isSafe();

                if(safe != wasSafe)
                {
                    Log(safe ? "OTA: accepting updates" : "OTA: updates paused, not idle on utility power");
                    wasSafe = safe;
                }

                if(safe)
                {
                    if(_restartPending)
                    {
                        Log("OTA: restarting to apply update");
                        vTaskDelay(pdMS_TO_TICKS(500));
                        ESP.restart();
                    }

                    ArduinoOTA.handle();
                }

                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
    };
}
#endif
