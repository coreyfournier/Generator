#include <stdint.h>

class VoltageDetector
{
    public:
    /**
     * @brief Construct a new Voltage Dector object
     * 
     * @param func function to read the analog pin "analogRead"
     * @param gpioPin A0-A12
     */
    VoltageDetector(uint16_t (*func)(uint8_t), uint8_t &gpioPin){
        
    }

    bool IsChanged()
    {
        return true;
    }

};