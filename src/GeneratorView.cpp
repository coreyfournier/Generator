#pragma once
#include <string>
#include <vector>


class Pin{
    public:
    int gpio;
    bool state;
    std::string name; 
    Pin(){};
    Pin(int gpio, bool state, std::string name)
    {
        this->gpio = gpio;
        this->state = state;
        this->name = name;
    };

};

class GeneratorView{
  public :
    std::vector<Pin> pins;
    int startGPIO;
    bool startState;

    GeneratorView()
    {

    };

};
