#include <unity.h>
#include <string>
#include <stdint.h>
#include "States/Orchestration.cpp"
#include "States/IEvent.cpp"
#include <stdio.h>
#include <iostream>

using namespace std;

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
/*

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

    //auto o = new Orchestration(analogReader, pin1, pin2, lh);

    //loop for a few times to simulate things happening over time
    for(int i =0; i< 10; i++)
    {
        //o->SenseChanges();
    }
    */
    

    TEST_ASSERT_EQUAL_STRING("Hello", "Hello");
}



int main()
{
    //delay(2000); // service delay
    UNITY_BEGIN();
    

    // EventStub es = EventStub();
    // IO::RtosIO board = IO::RtosIO();
    // IO::RtosQueue queue = IO::RtosQueue();
    // IO::RtosSerial s = IO::RtosSerial();
    // Orchestration view = Orchestration( 
    // &es, 
    // &board,
    // &queue,
    // &s);

    RUN_TEST(test_string_substring);
    UNITY_END(); // stop unit testing
}

void loop()
{
}