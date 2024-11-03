#include <unity.h>
#include <string>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include "IO/RingBuffer.cpp"
#include "States/test_orchestration.cpp"


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

    TEST_ASSERT_EQUAL_STRING("Hello", "Hello");
}

void test_ring_buffer(void) {
    
    auto rb = IO::RingBuffer<int>(5);

    for(int i = 0; i <= 100; i++)
    {
        rb.Add(i);
    }

    auto buffer = rb.GetBuffer();
    
    TEST_ASSERT_EQUAL_INT32(100, buffer.front());

    auto it = buffer.begin();
    advance(it,1);
    TEST_ASSERT_EQUAL_INT32(99, *it);
    advance(it,1);
    TEST_ASSERT_EQUAL_INT32(98, *it);
    advance(it,1);
    TEST_ASSERT_EQUAL_INT32(97, *it);

    TEST_ASSERT_EQUAL_INT32(4, buffer.size());
}


int main()
{
    //delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(test_orchestration);
    
    RUN_TEST(test_string_substring);
    RUN_TEST(test_ring_buffer);

    //RUN_TEST(test_state_change);
    UNITY_END(); // stop unit testing
}



void loop()
{
}