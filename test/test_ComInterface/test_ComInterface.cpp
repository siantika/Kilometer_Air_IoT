#include <Arduino.h>
#include <unity.h>
#include <SoftwareSerial.h>

#include "ComInterface.h"

ComInterface sim800;

SoftwareSerial sim800Serial(10, 11);

void setUp(void)
{
}

void tearDown(void)
{
}

// storing incoming data
// String receieveDataFromSerial(void)
// {
//     String _dataIn = "";
//     while (sim800Serial.available() > 0)
//     {
//         _dataIn = sim800Serial.readString();
//     }

//     return _dataIn;
// }

void test_example(void)
{
    TEST_ASSERT_EQUAL(1, 1);
}

void setup()
{
    delay(2000);

    UNITY_BEGIN();
    RUN_TEST(test_example);
    UNITY_END();
}

void loop()
{
}