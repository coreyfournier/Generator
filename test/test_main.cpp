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
    STR_TO_TEST = "Hello, world!";
}

void tearDown(void) {
    STR_TO_TEST = "";
}

void test_string_substring(void) {
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
    UNITY_BEGIN();

    RUN_TEST(test_simple_queue);
    RUN_TEST(test_string_substring);
    RUN_TEST(test_ring_buffer);

    // State machine end-to-end tests
    RUN_TEST(test_orchestration);
    RUN_TEST(test_utility_off_starts_generator);
    RUN_TEST(test_utility_returns_transfers_back);
    RUN_TEST(test_generator_start_failure_disables);
    RUN_TEST(test_disabled_does_nothing);
    RUN_TEST(test_utility_flicker_returns_to_idle);

    UNITY_END();
}


void loop()
{
}
