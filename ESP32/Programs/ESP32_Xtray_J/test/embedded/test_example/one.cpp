#include "test_helpers.h" // Useful test helpers for std::string, etl::string_view, floats, and more.

#include <Arduino.h>
#include <unity.h>


void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_equality()
{
    TEST_ASSERT_EQUAL(2, 1 + 1);
}

void setup()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_equality);
}


void loop()
{
    // Run as long as needed...
    // delay(1000);
    UNITY_END(); // stop unit testing
}