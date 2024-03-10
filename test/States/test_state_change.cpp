#include <unity.h>
#include <string>
#include <stdint.h>
#include "States/Orchestration.cpp"
#include "States/IEvent.cpp"
#include <stdio.h>
#include <iostream>
#include "IO/MockBoard.cpp"
#include "IO/SimpleQueue.cpp"
#include "IO/PrintSerial.cpp"
#include "IO/ISerial.h"
#include "States/ChangeMessage.cpp"
#include "States/UtilityOff.cpp"
#include "Devices/PowerDevice.cpp"


using namespace std;
using namespace States;

class FakeContext: public IContext
{
    IO::PrintSerial* print = new IO::PrintSerial();
    Pin* _utilityL1 = new Pin(1, false, "Utility L1", PinRole::UtilityOnL1);
    Pin* _generatorL1 = new Pin(3, false, "Generator L1", PinRole::GeneratorOnL1);

    Devices::PowerDevice* _generator = new Devices::PowerDevice(_generatorL1);
    Devices::PowerDevice* _utility = new Devices::PowerDevice(_utilityL1);

    public:
    FakeContext()
    {

    }

    void Delay(int milliseconds)
    {

    }

    Pin* FindByGpio(int gpio)
    {
        return nullptr;
    }

    Pin* FindByRole(PinRole role) {

        return nullptr;
    }

    IO::ISerial* GetSerialIO()
    {
        return this->print;
    }

    void StateChange(Event e, bool doAction)
    {
        this->print->Println(IEvent::ToName(e).c_str());
    }

    Devices::PowerDevice* GetUtility()
    {
        return nullptr;
    }
    
    Devices::PowerDevice* GetGenerator()
    {
        return nullptr;
    }
};


void test_state_change(void) {
    
    IO::MockBoard board = IO::MockBoard();
    IO::SimpleQueue queue = IO::SimpleQueue();
    IO::PrintSerial print = IO::PrintSerial();
    FakeContext* fc = new FakeContext();
    States::UtilityOff* uo = new States::UtilityOff(fc);

    uo->DoAction();

    UnityPrint("test_state_change test here\n");

    TEST_ASSERT_EQUAL_STRING("Hello", "Hello");
}