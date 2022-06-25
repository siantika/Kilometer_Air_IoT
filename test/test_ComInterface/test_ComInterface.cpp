#include <Arduino.h>
#include <unity.h>

#include "ComInterface.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_example(void)
{
    TEST_ASSERT_EQUAL (1,1);
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