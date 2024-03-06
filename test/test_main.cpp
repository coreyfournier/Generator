#include <unity.h>
#include <string>
#include <stdint.h>
#include "States/Orchestration.cpp"
#include "States/IEvent.cpp"
#include <stdio.h>
#include <iostream>
#include "IO/MockBoard.cpp"
#include "IO/MockQueue.cpp"
#include "IO/PrintSerial.cpp"
#include "IO/ISerial.h"

using namespace std;
using namespace States;

string STR_TO_TEST;

void setUp(void) {
    // set stuff up here
    STR_TO_TEST = "Hello, world!";
}

void tearDown(void) {
    // clean stuff up here
    STR_TO_TEST = "";
}


void test_string_substring(void) {
    
    cout << "\n*************** test_string_substring *********************\n\n";

    UnityPrint("some test here\n");
    uint8_t pin1 = 1;
    uint8_t pin2 = 2;


    class ListnerHandler: public IEvent
    {
        public:
        void Change(Event e)
        {
            fprintf(stdout, "gpio_master_test %i", e);
            std:cout << "gpio_master_test " << e;
            //Serial.printf("gpio_master_test %i", e);
        }
    };

    ListnerHandler *lh = new ListnerHandler();
    
    auto analogReader = [] (uint8_t gpioPin) -> uint16_t
    {
        return 1;
    };    
    
    IO::MockBoard board = IO::MockBoard();
    IO::MockQueue queue = IO::MockQueue();
    IO::PrintSerial print = IO::PrintSerial();
    States::Orchestration view = States::Orchestration( 
    lh, 
    &board,
    &queue,
    &print);

    Pin L1OnSense = Pin(1, false, "Utility L1 on/off", true, PinRole::UtilityOnL1);
    Pin L2OnSense = Pin(2, false, "Utility L2 on/off", true, PinRole::UtilityOnL2);
    Pin generatorOnSense = Pin(3, false, "Generator on/off", true, PinRole::GeneratorOnL1);
    view.AddPin(L1OnSense);
    view.AddPin(L2OnSense);
    view.AddPin(generatorOnSense);
    view.AddPin(Pin(4, false, "Transfer", PinRole::Transfer));
    view.AddPin(Pin(5, false, "Start Generator", PinRole::Start));
    view.AddPin(Pin(6, false, "Stop Generator", PinRole::Stop));

    view.Initalize();

    TEST_ASSERT_EQUAL_STRING("Hello", "Hello");
}



int main()
{
    //delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(test_string_substring);
    UNITY_END(); // stop unit testing
}

void loop()
{
}