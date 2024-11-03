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

void test_simple_queue()
{
    IO::SimpleQueue<States::ChangeMessage>* stateQueue = new IO::SimpleQueue<States::ChangeMessage>();
    auto change = new States::ChangeMessage(States::Event::Generator_On, 5);
    stateQueue->QueueMessage(change);

    auto message = stateQueue->BlockAndDequeue();

    TEST_ASSERT(change->event == message->event);
    TEST_ASSERT(change->time == message->time);
}

void test_orchestration()
{
    //IO::MockBoard board = IO::MockBoard();
    IO::SimpleQueue<States::ChangeMessage>* stateQueue = new IO::SimpleQueue<States::ChangeMessage>();
    IO::SimpleQueue<States::PinChange>* pinQueue = new IO::SimpleQueue<States::PinChange>();

    IO::PrintSerial print = IO::PrintSerial();
    Pin L1OnSense = Pin(1, false, "Utility L1 on/off", true, PinRole::UtilityOnL1);
    Pin L2OnSense = Pin(2, false, "Utility L2 on/off", true, PinRole::UtilityOnL2);
    Pin generatorOnSense = Pin(3, false, "Generator on/off", true, PinRole::GeneratorOnL1);
    
 


    
    /*
    States::Orchestration view = States::Orchestration( 
        lh, 
        &board,
        &queue,
        &print);

    
    view.AddPin(L1OnSense);
    view.AddPin(L2OnSense);
    view.AddPin(generatorOnSense);
    view.AddPin(Pin(4, false, "Transfer", PinRole::Transfer));
    view.AddPin(Pin(5, false, "Start Generator", PinRole::Start));
    view.AddPin(Pin(6, false, "Stop Generator", PinRole::Stop));
    */
}