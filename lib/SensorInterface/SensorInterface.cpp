

#include "SensorInterface.h"

SensorInterface ::SensorInterface()
{
    // pass
}

// Derived Class
/* *********** WaterFlow Class *********** */
volatile uint16_t WaterFlow::sWater_pulse; // an instance to Interrupt function

// Private Methods start here
float WaterFlow::getWaterFlow()
{
    float mQ; // Water flows in m3/sec unit.
    mQ = CUBIC_CONST * sWater_pulse;
    sWater_pulse = 0.0;
    return mQ;
}

void WaterFlow::sCountPulse()
{
    sWater_pulse++;
}
/* Private Methods ends here */

// Public Methods start here
WaterFlow::WaterFlow(uint8_t mSensor_pin)
{
    this->mSensor_pin = mSensor_pin;
    sWater_pulse = 0;
}

void WaterFlow::init()
{
    pinMode(mSensor_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(mSensor_pin), WaterFlow::sCountPulse, RISING);
}

float WaterFlow::getWaterVolume()
{
    return getWaterFlow();
}

bool WaterFlow::setVolumeAlarm(uint16_t time_threshold, uint16_t on_going_time)
{
    if (on_going_time >= time_threshold)
        return 1;
    else
        return 0;
}
/* Public methods end here */

/* *********** WaterFlow Class ends here *********** */

/* *********** Battery Level Class *********** */
// Constructor
Battery::Battery(uint8_t mSensor_pin)
{
    this->mSensor_pin = mSensor_pin;
}

// private methods
uint16_t Battery::readAnalogData()
{
    return mAnalog_data = analogRead(mSensor_pin); // changeeee
}

// public methods
float Battery::getVoltage(float mVrefPin)
{
    // src: https://www.electronicshub.org/interfacing-voltage-sensor-with-arduino/#Code
    // Resistors value
    float mVoltage_in; // input voltage from sensor
    float mVoltage_read;
    const float mR1 = 19500.0; // ohm (MAKE SURE THE VALUE OF THE RESISTOR IS PRECISE!!!)
    const float mR2 = 4564.0;  // ohm

    mAnalog_data = Battery::readAnalogData(); //

    mVoltage_in = mAnalog_data / ADC_RESOLUTION * mVrefPin;
    mVoltage_read = mVoltage_in / (mR2 / (mR1 + mR2)); // converting to volt

    return mVoltage_read;
}