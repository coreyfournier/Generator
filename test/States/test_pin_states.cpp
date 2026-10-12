#include <unity.h>
#include <vector>
#include "IO/PinTopic.h"
#include "IO/NotifyingBoard.cpp"
#include "IO/IPinStateListner.h"
//Uses TestHarness from States/test_orchestration.cpp, included before this file by test_main.cpp.

/// @brief Records every pin change it is told about.
struct RecordingPinListener : public IO::IPinStateListner
{
    std::vector<IO::Pin> changes;

    void OnPinChanged(const IO::Pin& pin)
    {
        changes.push_back(pin);
    }
};

/// @brief Topics are keyed by role, so they stay put if a pin moves to another GPIO.
void test_pin_topic_keys()
{
    TEST_ASSERT_EQUAL_STRING("utility_l1", IO::PinTopicKey(PinRole::UtilityOnL1, 15).c_str());
    TEST_ASSERT_EQUAL_STRING("utility_l2", IO::PinTopicKey(PinRole::UtilityOnL2, 32).c_str());
    TEST_ASSERT_EQUAL_STRING("generator_l1", IO::PinTopicKey(PinRole::GeneratorOnL1, 14).c_str());
    TEST_ASSERT_EQUAL_STRING("transfer", IO::PinTopicKey(PinRole::Transfer, 12).c_str());
    TEST_ASSERT_EQUAL_STRING("start", IO::PinTopicKey(PinRole::Start, 27).c_str());
    TEST_ASSERT_EQUAL_STRING("stop", IO::PinTopicKey(PinRole::Stop, 27).c_str());
    TEST_ASSERT_EQUAL_STRING("gpio_33", IO::PinTopicKey(PinRole::Other, 33).c_str());
    TEST_ASSERT_EQUAL_STRING("ON", IO::PinPayload(true));
    TEST_ASSERT_EQUAL_STRING("OFF", IO::PinPayload(false));
}

/// @brief Every write through the board is reported with the new state; reads are not.
void test_notifying_board_reports_writes()
{
    IO::MockBoard inner;
    IO::NotifyingBoard board(&inner);
    RecordingPinListener listener;
    board.AddListener(&listener);
    IO::Pin transfer(4, false, "Transfer", PinRole::Transfer);

    board.DigitalWrite(transfer, true);
    board.DigitalRead(transfer);
    board.DigitalWrite(transfer, false);

    TEST_ASSERT_EQUAL_INT(2, (int)listener.changes.size());
    TEST_ASSERT_TRUE(listener.changes[0].state);
    TEST_ASSERT_FALSE(listener.changes[1].state);
    TEST_ASSERT_EQUAL_INT((int)PinRole::Transfer, (int)listener.changes[0].role);
    TEST_ASSERT_TRUE(inner.GetWrittenValue(4) == false);
}

/// @brief A debounced input change updates the pin and is reported; re-reading an unchanged pin is not.
void test_input_change_notifies_pin_listener()
{
    TestHarness h;
    RecordingPinListener listener;
    h.orchestration->AddPinStateListener(&listener);
    h.SetUtilityOn(true);
    h.orchestration->Initalize();
    h.orchestration->DrainAllStateChanges();

    //Utility drops: the interrupt queues a read of the pin
    h.board.SetPinValue(h.utilityL1.gpio, false);
    h.board.AdvanceTicks(1000);
    States::PinChange drop(&h.utilityL1, h.board.TicksOfTime(), true);
    h.orchestration->ProcessPinChange(&drop);

    TEST_ASSERT_EQUAL_INT(1, (int)listener.changes.size());
    TEST_ASSERT_EQUAL_INT((int)PinRole::UtilityOnL1, (int)listener.changes[0].role);
    TEST_ASSERT_FALSE(listener.changes[0].state);
    TEST_ASSERT_FALSE(h.utilityL1.state);

    //Same value read again: nothing changed, nothing reported
    h.board.AdvanceTicks(1000);
    States::PinChange same(&h.utilityL1, h.board.TicksOfTime(), true);
    h.orchestration->ProcessPinChange(&same);

    TEST_ASSERT_EQUAL_INT(1, (int)listener.changes.size());
}

/// @brief Writes made by the system are reported by the board, so the state machine doesn't repeat them.
void test_system_write_not_reported_twice()
{
    TestHarness h;
    RecordingPinListener listener;
    h.orchestration->AddPinStateListener(&listener);
    h.SetUtilityOn(true);
    h.orchestration->Initalize();
    h.orchestration->DrainAllStateChanges();

    h.board.AdvanceTicks(1000);
    h.transfer.state = true;
    States::PinChange write(&h.transfer, h.board.TicksOfTime(), false);
    h.orchestration->ProcessPinChange(&write);

    TEST_ASSERT_EQUAL_INT(0, (int)listener.changes.size());
}

/// @brief Changes inside the debounce window are ignored, matching the state machine.
void test_input_change_inside_debounce_ignored()
{
    TestHarness h;
    RecordingPinListener listener;
    h.orchestration->AddPinStateListener(&listener);
    h.SetUtilityOn(true);
    h.orchestration->Initalize();
    h.orchestration->DrainAllStateChanges();

    h.board.SetPinValue(h.generatorL1.gpio, true);
    h.board.AdvanceTicks(1000);
    States::PinChange first(&h.generatorL1, h.board.TicksOfTime(), true);
    h.orchestration->ProcessPinChange(&first);

    h.board.SetPinValue(h.generatorL1.gpio, false);
    h.board.AdvanceTicks(MinTimeAllowedBetweenMessages / 2);
    States::PinChange bounce(&h.generatorL1, h.board.TicksOfTime(), true);
    h.orchestration->ProcessPinChange(&bounce);

    TEST_ASSERT_EQUAL_INT(1, (int)listener.changes.size());
    TEST_ASSERT_TRUE(listener.changes[0].state);
}

/// @brief Starting the generator through a notifying board reports the momentary start press (on, then off).
void test_generator_start_reports_start_pulse()
{
    IO::MockBoard inner;
    IO::NotifyingBoard board(&inner);
    RecordingPinListener listener;
    board.AddListener(&listener);
    IO::Pin sense(3, false, "Generator L1", true, PinRole::GeneratorOnL1);
    IO::Pin start(5, false, "Start", PinRole::Start);
    Devices::StartableDevice generator(&sense, nullptr, &start, nullptr, &board, true);

    generator.Start();

    TEST_ASSERT_EQUAL_INT(2, (int)listener.changes.size());
    TEST_ASSERT_EQUAL_INT((int)PinRole::Start, (int)listener.changes[0].role);
    TEST_ASSERT_TRUE(listener.changes[0].state);
    TEST_ASSERT_FALSE(listener.changes[1].state);
}
