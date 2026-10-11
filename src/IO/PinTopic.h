#pragma once
#include <string>
#include "PinRole.cpp"

namespace IO
{
    /// @brief Topic segment for a pin, published as <MqttBaseTopic>/<DeviceHostName>/pin/<key>.
    /// Keyed by role so the topic stays the same if the pin moves to another GPIO.
    inline std::string PinTopicKey(PinRole role, int gpio)
    {
        switch(role)
        {
            case PinRole::Start: return "start";
            case PinRole::Stop: return "stop";
            case PinRole::Transfer: return "transfer";
            case PinRole::UtilityOnL1: return "utility_l1";
            case PinRole::UtilityOnL2: return "utility_l2";
            case PinRole::GeneratorOnL1: return "generator_l1";
            case PinRole::GeneratorOnL2: return "generator_l2";
            default: return "gpio_" + std::to_string(gpio);
        }
    }

    /// @brief Pin state payload, matching Home Assistant's default binary sensor payloads.
    inline const char* PinPayload(bool state)
    {
        return state ? "ON" : "OFF";
    }
}
