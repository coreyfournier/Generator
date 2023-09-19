#include <unity.h>
#include <string>
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
    TEST_ASSERT_EQUAL_STRING("Hello", "");
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