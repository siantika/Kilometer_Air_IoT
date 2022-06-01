#ifndef SENSORINTERFACE_H
#define SENSORINTERFACE_H

#include <Arduino.h>
#include "header.h"


class SensorInterface
{
protected:
uint8_t _sensorPin;
   
public:
    SensorInterface();
};

// Derived class 
class WaterFlow : SensorInterface{
    static volatile uint16_t _waterPulse; // an interrupt variable has to static type. Reference: https://arduino.stackexchange.com/questions/73287/interrupts-inside-a-class-attaching-to-the-function-of-the-class
    private:
    // variables
    const float WATER_CONST =1.66667E-5; // it produces water flow's value in m3/sec
    // Methods
    static void countPulse();

    public:
    // Methods
    WaterFlow(uint8_t _sensorPin); // constructor

    float getWaterflow();    
    void init(uint8_t _sensorPin);
    bool setVolumealarm(uint16_t timeThreshold, uint16_t onGoingTime);
    float getWatervolume(void);

};

// Derived Class
class BatteryLevel : SensorInterface{
    private:
    uint16_t _analogData;
    uint16_t readAnalogdata();


    public:
    BatteryLevel (uint8_t _sensorPin);
    float getVoltage();
};

#endif