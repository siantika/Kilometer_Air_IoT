#ifndef SENSORINTERFACE_H
#define SENSORINTERFACE_H

#include <Arduino.h>
#include "header.h"

class SensorInterface
{
protected:
    uint8_t mSensor_pin;

public:
    SensorInterface();
};

// Derived class
class WaterFlow : SensorInterface
{
    static volatile uint16_t sWater_pulse; // an interrupt variable has to be static type. Reference: https://arduino.stackexchange.com/questions/73287/interrupts-inside-a-class-attaching-to-the-function-of-the-class

private:
    // variables
    const float CUBIC_CONST = 1515E-6; // it produces water flow's value in m3/sec
    // Methods
    static void sCountPulse();

public:
    // Methods
    // block another data type
    explicit WaterFlow(uint8_t mSensor_pin);
    float getWaterFlow();
    void init();
    bool setVolumeAlarm(uint16_t time_threshold, uint16_t on_going_time);
    float getWaterVolume();
};

// Derived Class
class Battery : SensorInterface
{
private:
    uint16_t mAnalog_data;
    uint16_t readAnalogData();

public:
    explicit Battery(uint8_t mSensor_pin);
    float getVoltage(float mVrefPin);
};

#endif