#include "SensorInterface.h"


SensorInterface ::SensorInterface(){
 // pass
}


// Derived Classess

/* *********** WaterFlow Class *********** */

volatile uint16_t WaterFlow::_waterPulse; // an instance to Interrupt function

// Private Methods start here

float WaterFlow::getWaterflow(){
    float _Q; // Water flows in m3/sec unit.
    uint16_t _freqPulse = _waterPulse;
    _Q = WATER_CONST *  _freqPulse;
    _waterPulse = 0.0;
    return _Q;
}

void WaterFlow::countPulse(){
   _waterPulse ++;
}
/* Private Methods ends here */


// Public Methods start here
WaterFlow:: WaterFlow(uint8_t SensorPin){
    _sensorPin = SensorPin;
    _waterPulse = 0;
}

void WaterFlow::init(uint8_t _sensorPin){
    // declare interrupt mode
    pinMode(_sensorPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_sensorPin), WaterFlow::countPulse , RISING);
}

float WaterFlow::getWatervolume(){
    return getWaterflow();
}

bool WaterFlow::setVolumealarm(uint16_t timeThreshold, uint16_t onGoingTime){
    if (onGoingTime >= timeThreshold)  return 1;
    else return 0;
}
/* Public methods end here */

/* *********** WaterFlow Class ends here *********** */


/* *********** Battery Level Class *********** */
// Constructor
BatteryLevel::BatteryLevel(uint8_t _sensorPin){
    // initilize pin mode
    pinMode(_sensorPin, INPUT);
}

// private methods
uint16_t BatteryLevel::readAnalogdata(){
 return _analogData = analogRead(_sensorPin);
}

// public methods
float BatteryLevel::getVoltage(){
    // equation
    return (_analogData/ADC_RESOLUTION * V_REF_5V); // NEED to find RIGHT EQUATION !!!!
}