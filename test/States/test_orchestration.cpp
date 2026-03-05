#include <unity.h>
#include <string>
#include <stdint.h>
#include "States/IEvent.cpp"
#include <stdio.h>
#include <iostream>
#include "IO/MockBoard.cpp"
#include "IO/SimpleQueue.cpp"
#include "IO/PrintSerial.cpp"
#include "IO/ISerial.h"

#include "IO/Pin.cpp"
#include "States/Orchestration.cpp"
#include "States/ChangeMessage.cpp"
#include "States/PinChange.cpp"
#include "Devices/PowerDevice.cpp"
#include "Devices/StartableDevice.cpp"
#include "Devices/TransferSwitch.cpp"

/// @brief Helper to create an Orchestration with controllable mock components.
struct TestHarness
{
    IO::MockBoard board;
    IO::SimpleQueue<States::ChangeMessage> stateQueue;
    IO::SimpleQueue<States::PinChange> pinQueue;
    IO::PrintSerial serial;

    Pin utilityL1;
    Pin generatorL1;
    Pin transfer;
    Pin genStart;
    Pin genStop;

    Devices::PowerDevice* utility;
    Devices::StartableDevice* generator;
    Devices::TransferSwitch* transferSwitch;
    States::Orchestration* orchestration;

    TestHarness(States::Event initialState = States::Event::Initalize) :
        utilityL1(1, false, "Utility L1", true, PinRole::UtilityOnL1),
        generatorL1(3, false, "Generator L1", true, PinRole::GeneratorOnL1),
        transfer(4, false, "Transfer", PinRole::Transfer),
        genStart(5, false, "Start", PinRole::Start),
        genStop(6, false, "Stop", PinRole::Stop)
    {
        utility = new Devices::PowerDevice(&utilityL1, &board);
        generator = new Devices::StartableDevice(
            &generatorL1, nullptr, &genStart, &genStop, &board, true);
        transferSwitch = new Devices::TransferSwitch(&transfer, &board, &serial);

        orchestration = new States::Orchestration(
            utility, generator, transferSwitch,
            &board, &stateQueue, &pinQueue, &serial,
            initialState);
    }

    ~TestHarness()
    {
        delete orchestration;
        delete transferSwitch;
        delete generator;
        delete utility;
    }

    void SetUtilityOn(bool on)
    {
        board.SetPinValue(utilityL1.gpio, on);
        utilityL1.state = on;
    }

    void SetGeneratorOn(bool on)
    {
        board.SetPinValue(generatorL1.gpio, on);
        generatorL1.state = on;
    }

    /// @brief Process queued messages one at a time. When the target event is
    /// reached, the callback fires BEFORE that state's DoAction executes.
    /// This allows tests to set mock pin values at the right moment.
    States::Event DrainUntilThen(States::Event target, void(*callback)(TestHarness*))
    {
        bool callbackFired = false;

        while(true)
        {
            States::Event nextEvent;
            if(!orchestration->PeekNextEvent(nextEvent))
                break;

            if(nextEvent == target && !callbackFired && callback != nullptr)
            {
                callback(this);
                callbackFired = true;
            }

            orchestration->ExecuteCurrentState();
        }

        return orchestration->GetState();
    }
};

void test_simple_queue()
{
    IO::SimpleQueue<States::ChangeMessage>* stateQueue = new IO::SimpleQueue<States::ChangeMessage>();
    auto change = new States::ChangeMessage(States::Event::Generator_On, 5);
    stateQueue->QueueMessage(change);

    auto message = stateQueue->BlockAndDequeue();

    TEST_ASSERT(change->event == message->event);
    TEST_ASSERT(change->time == message->time);

    delete stateQueue;
}

/// @brief When utility is on at startup, system should go to Idle
void test_orchestration()
{
    TestHarness h;
    h.SetUtilityOn(true);
    h.SetGeneratorOn(false);

    h.orchestration->Initalize();
    States::Event finalState = h.orchestration->DrainAllStateChanges();

    TEST_ASSERT_EQUAL_INT((int)States::Event::Idle, (int)finalState);
}

/// @brief Full power outage: utility off -> generator starts -> warm up -> transfer -> idle
void test_utility_off_starts_generator()
{
    TestHarness h;
    h.SetUtilityOn(false);
    h.SetGeneratorOn(false);

    h.orchestration->Initalize();

    // When GeneratorStart runs, it tries to start the generator then checks IsOn().
    // Simulate the generator coming on when Generator_Start state is reached.
    States::Event finalState = h.DrainUntilThen(
        States::Event::Generator_Start,
        [](TestHarness* h) { h->SetGeneratorOn(true); });

    TEST_ASSERT_EQUAL_INT((int)States::Event::Idle, (int)finalState);
    TEST_ASSERT_TRUE(h.transferSwitch->IsOnGenerator());
}

/// @brief Utility returns: transfer back to utility, stop generator, idle
void test_utility_returns_transfers_back()
{
    // Setup: generator running, transferred to generator
    TestHarness h;
    h.SetUtilityOn(false);
    h.SetGeneratorOn(true);

    h.orchestration->Initalize();

    // UtilityOff sees generator already on -> GeneratorOn -> warm up -> transfer -> idle
    States::Event state = h.orchestration->DrainAllStateChanges();
    TEST_ASSERT_EQUAL_INT((int)States::Event::Idle, (int)state);
    TEST_ASSERT_TRUE(h.transferSwitch->IsOnGenerator());

    // Now utility comes back
    h.SetUtilityOn(true);
    h.orchestration->StateChange(States::Event::Utility_On);

    // GeneratorStop will try to stop the generator. Simulate it turning off.
    state = h.DrainUntilThen(
        States::Event::Generator_Stop,
        [](TestHarness* h) { h->SetGeneratorOn(false); });

    TEST_ASSERT_EQUAL_INT((int)States::Event::Idle, (int)state);
    TEST_ASSERT_FALSE(h.transferSwitch->IsOnGenerator());
}

/// @brief Generator fails to start after max attempts -> Disabled
void test_generator_start_failure_disables()
{
    TestHarness h;
    h.SetUtilityOn(false);
    h.SetGeneratorOn(false); // Never comes on

    h.orchestration->Initalize();
    States::Event finalState = h.orchestration->DrainAllStateChanges();

    TEST_ASSERT_EQUAL_INT((int)States::Event::Disabled, (int)finalState);
}

/// @brief Starting in Disabled mode does nothing
void test_disabled_does_nothing()
{
    TestHarness h(States::Event::Disabled);
    h.SetUtilityOn(true);

    h.orchestration->Initalize();
    States::Event finalState = h.orchestration->DrainAllStateChanges();

    TEST_ASSERT_EQUAL_INT((int)States::Event::Disabled, (int)finalState);
}

/// @brief Utility flickers off then back on during wait period -> returns to Idle
void test_utility_flicker_returns_to_idle()
{
    TestHarness h;
    h.SetUtilityOn(false);
    h.SetGeneratorOn(false);

    h.orchestration->Initalize();

    // When UtilityOff runs, it waits then checks IsOn() again.
    // Simulate utility coming back during the wait (before Generator_Start).
    States::Event finalState = h.DrainUntilThen(
        States::Event::Utility_Off,
        [](TestHarness* h) { h->SetUtilityOn(true); });

    // UtilityOff checks IsOn() after wait - sees utility back, queues Utility_On.
    // UtilityOn sees utility on, no transfer needed, goes to Idle.
    TEST_ASSERT_EQUAL_INT((int)States::Event::Idle, (int)finalState);
    TEST_ASSERT_FALSE(h.transferSwitch->IsOnGenerator());
}
