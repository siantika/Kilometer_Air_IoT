#include <Arduino.h>
#include <unity.h>
#include "IndicatorInterface.h"
#include "io_mapping.h"

// macro define for test
#define BUTTON_PRESSED_TO_LOW

IndicatorInterface<TypeEnum::__OUTPUT> test_ledIndicator{PIN_LED_INDICATOR};
IndicatorInterface<TypeEnum::__INPUT> test_Button_opt{PIN_BUTTON_OPT};


void setUp(void)
{
}

void tearDown(void)
{
}


// function tests

void test_indicatorLed_given_high_input_should_be_on(void)
{
    test_ledIndicator.turnOn();
    TEST_ASSERT_EQUAL(HIGH, digitalRead(PIN_LED_INDICATOR));
}

void test_indicatorLed_given_low_input_should_be_off(void)
{
    test_ledIndicator.turnOff();
    TEST_ASSERT_EQUAL(0, digitalRead(PIN_LED_INDICATOR));
}

// button opeartion test scope

void test_buttonOpt_operation(void)
{
#ifdef BUTTON_PRESSED_TO_HIGH
    test_Button_opt.getInputDigital();
    TEST_ASSERT_EQUAL(1, digitalRead(PIN_BUTTON_OPT));

#endif

#ifdef BUTTON_UNPRESSED
    // due to input pullup mode .
    test_Button_opt.getInputDigital();
    TEST_ASSERT_EQUAL(1, digitalRead(PIN_BUTTON_OPT));

#endif

#ifdef BUTTON_PRESSED_TO_LOW
    test_Button_opt.getInputDigital();
    TEST_ASSERT_EQUAL(0, digitalRead(PIN_BUTTON_OPT));

#endif
}

void setup()
{
    delay(2000);

    UNITY_BEGIN();

    RUN_TEST(test_indicatorLed_given_high_input_should_be_on);
    RUN_TEST(test_indicatorLed_given_low_input_should_be_off);

    RUN_TEST(test_buttonOpt_operation);

    UNITY_END();
}

void loop()
{
}