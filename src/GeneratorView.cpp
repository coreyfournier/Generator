#pragma once
#include <string>
#include <vector>


class Pin{
    public:
    /// @brief GPIO number on the board
    int gpio;
    /// @brief The pin is either high/ True or low /False
    bool state;
    /// @brief Name to display for this pin
    std::string name; 
    /// @brief If it's readonly then the pin can't be set
    bool isReadOnly;
    Pin(){};
    Pin(int gpio, bool state, std::string name)
    {
        this->gpio = gpio;
        this->state = state;
        this->name = name;
        this->isReadOnly = false;
    };

    Pin(int gpio, bool state, std::string name, bool isReadOnly)
    {
        this->gpio = gpio;
        this->state = state;
        this->name = name;
        this->isReadOnly = isReadOnly;
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
