#pragma once
#include <string>
#include <vector>
#include "Pin.cpp"

class GeneratorView{
  public :
    std::vector<Pin> pins;
    int startGPIO;
    bool startState;

    GeneratorView()
    {

    };

    /// @brief Looks for the specified pin in the list. If not found nullptr is returned.
    /// @param gpio 
    /// @return Pin or nullptr
    Pin* FindByGpio(int gpio)
    {
        for(int i=0; i< pins.size(); i++)
        {
            if(gpio == pins[i].gpio)
                return &pins[i];
        }

        return nullptr;
    }

};
