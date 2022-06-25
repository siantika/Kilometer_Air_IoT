/* ***************************************************************************************** */
/* IndicatorInterface class                                                                  */   
/* It provides hardware indicator methods                                                    */ 
/* Created by: I Putu Pawesi Siantika, S.T.                                                  */
/* Src: https://stackoverflow.com/questions/21648237/c-enable-disable-class-member-functions */
/* ***************************************************************************************** */

// it uses conditional function using template class
// there are 2 conditions contained in this class. Indicator for INPUT and for OUTPUT.
// OUTPUT is represented by 0 and INPUT represented by 1. please check the src link above.

#include "IndicatorInterface.h"

// For INPUT MODE
IndicatorInterface<TypeEnum::__INPUT>::IndicatorInterface(uint8_t mIndicator_pin){
   this-> mIndicator_pin = mIndicator_pin;
   pinMode(mIndicator_pin, INPUT_PULLUP);
}

bool IndicatorInterface<TypeEnum::__INPUT>::getInputDigital(void){
    return digitalRead(mIndicator_pin);
}

// For OUTPUT MODE
IndicatorInterface<TypeEnum::__OUTPUT>::IndicatorInterface(uint8_t mIndicator_pin){
   this-> mIndicator_pin = mIndicator_pin;
   pinMode(mIndicator_pin, OUTPUT);
}

bool IndicatorInterface<TypeEnum::__OUTPUT>::turnOn(void){
    digitalWrite(mIndicator_pin, HIGH);
    return 1;
}

bool IndicatorInterface<TypeEnum::__OUTPUT>::turnOff(void){
    digitalWrite(mIndicator_pin, LOW);
    return 0;
}