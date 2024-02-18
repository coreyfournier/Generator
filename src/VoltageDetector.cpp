#pragma once
#include <stdint.h>

/// @brief 
class VoltageDetector
{
    private:
    bool _lastState = false;
    uint16_t (*_analogReader)(uint8_t);
    uint8_t _gpioPin;
    
    public:
    /**
     * @brief Construct a new Voltage Dector object
     * 
     * @param func function to read the analog pin "analogRead"
     * @param gpioPin A0-A12
     */
    VoltageDetector(uint16_t (*func)(uint8_t), uint8_t &gpioPin){
        _analogReader = func;
        _gpioPin = gpioPin;
    }

    bool IsChanged()
    {
        return true;
    }

    bool IsOn()
    {
        return true;
    }

};