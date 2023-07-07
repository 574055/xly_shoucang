#include "filters.h"
#include "sensing.h"
#include "test_helpers.h"

#include <Arduino.h>
#include <unity.h>


#define STABLE_ITERS                50
#define STABLE_THRESHOLD            40.0 // TODO: tune this?
#define STABLE_ATTENUATED_THRESHOLD 0.01 // TODO: tune this?


void test_light_available()
{
    // Test if the light sensor is available using the library function.
    TEST_ASSERT_TRUE(Sensors.light.isAvailable());

    // Check values aren't inf or nan. (i.e. make sure they are valid float numbers).
    TEST_ASSERT_FLOAT_IS_DETERMINATE(Sensors.light.getRaw());
    TEST_ASSERT_FLOAT_IS_DETERMINATE(Sensors.light.get());
}

void test_light_stable()
{
    // Test if the light sensor data is stable.
    Filter::Statistical filtr; // raw
    Filter::Statistical filta; // attenuated
    for (int i = 0; i < STABLE_ITERS; i++) {
        filtr.apply(Sensors.light.getRaw());
        filta.apply(Sensors.light.get());
    }

    Serial.println("Light");
    Serial.printf("filter raw | mean: %8.3f | var: %8.3f\n", filtr.mean(), filtr.var());
    Serial.printf("filter atn | mean: %8.3f | var: %8.3f\n", filta.mean(), filta.var());

    TEST_ASSERT_LT_FLOAT(filtr.var(), STABLE_THRESHOLD);
    TEST_ASSERT_LT_FLOAT(filta.var(), STABLE_ATTENUATED_THRESHOLD);
}


void test_temperature_available()
{
    // Test if the temperature sensor is available using the library function.
    TEST_ASSERT_TRUE(Sensors.temp.isAvailable());
    TEST_ASSERT_FLOAT_IS_DETERMINATE(Sensors.temp.get());
}

void test_temperature_stable()
{
    // Test if the temperature sensor data is stable.
    Filter::Statistical filt; // attenuated
    for (int i = 0; i < STABLE_ITERS; i++) {
        filt.apply(Sensors.temp.get());
    }

    Serial.println("Temperature");
    Serial.printf("filter | mean: %8.3f | var: %8.3f\n", filt.mean(), filt.var());

    TEST_ASSERT_LT_FLOAT(filt.var(), STABLE_THRESHOLD);
}


void test_humidity_available()
{
    // Test if the humidity sensor is available using the library function.
    TEST_ASSERT_TRUE(Sensors.humi.isAvailable());
    TEST_ASSERT_FLOAT_IS_DETERMINATE(Sensors.humi.get());
}

void test_humidity_stable()
{
    // Test if the humidity sensor data is stable.
    Filter::Statistical filt; // attenuated
    for (int i = 0; i < STABLE_ITERS; i++) {
        filt.apply(Sensors.humi.get());
    }

    Serial.println("Humidity");
    Serial.printf("filter | mean: %8.3f | var: %8.3f\n", filt.mean(), filt.var());

    TEST_ASSERT_LT_FLOAT(filt.var(), STABLE_THRESHOLD);
}


void setup()
{
    Sensors.init();
    Serial.begin(115200);

    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);


    UNITY_BEGIN(); // IMPORTANT LINE!

    RUN_TEST(test_light_available);
    RUN_TEST(test_light_stable);

    RUN_TEST(test_temperature_available);
    RUN_TEST(test_temperature_stable);

    RUN_TEST(test_humidity_available);
    RUN_TEST(test_humidity_stable);

    UNITY_END(); // stop unit testing
}


void loop() {}