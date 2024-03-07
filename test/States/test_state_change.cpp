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
#include "States/ChangeMessage.cpp"
#include "States/UtilityOff.cpp"

using namespace std;
using namespace States;

class FakeContext: public IContext
{
    IO::PrintSerial* print = new IO::PrintSerial();

    public:
    Pin* FindByGpio(int gpio)
    {
        return new Pin();
    }

    Pin* FindByRole(PinRole role) {

        return new Pin();
    }

    IO::ISerial* GetSerialIO()
    {
        return this->print;
    }

    void StateChange(Event e)
    {

    }
};


void test_state_change(void) {
    
    IO::MockBoard board = IO::MockBoard();
    IO::MockQueue queue = IO::MockQueue();
    IO::PrintSerial print = IO::PrintSerial();
    FakeContext fc* = new FakeContext();
    States::UtilityOff uo* = new States::UtilityOff(fc);

    uo->DoAction();

    TEST_ASSERT_EQUAL_STRING("Hello", "Hello");
}


void setUp(void) {
    
}

void tearDown(void) {
    
}

int main()
{
    //delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(test_state_change);
    UNITY_END(); // stop unit testing
}

void loop()
{
}


