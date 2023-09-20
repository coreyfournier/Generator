#include <stdint.h>
#include "VoltageDetector.cpp"



using namespace std;

class Orchestration
{
    private:
    VoltageDetector *_utilitySense;
    VoltageDetector *_generatorSense;


    public:
    Orchestration(
        uint16_t (*func)(uint8_t),
        uint8_t &utilitySenseGpio,
        uint8_t &generatorSenseGpio
    ) 
    {
        _utilitySense = new VoltageDetector(func, utilitySenseGpio);
        _generatorSense = new VoltageDetector(func, generatorSenseGpio);
    }

    void SenseChanges()
    {
        
        if(_utilitySense->IsChanged())
        {

        }

        if(_generatorSense->IsChanged())
        {

        }
        
        
    }
};