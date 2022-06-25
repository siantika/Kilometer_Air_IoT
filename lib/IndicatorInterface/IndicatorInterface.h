/*
 This class contains 2 type depend on indicator mode (INPUT or OUTPUT).


 */

// declaring type

#ifndef INDICATOR_INTERFACE_H
#define INDICATOR_INTERFACE_H

#include <Arduino.h>

enum struct TypeEnum : bool {
    __OUTPUT = 0,
    __INPUT  = 1 
};

template <TypeEnum Type>
    class IndicatorInterface;
template<>

// For INPUT MODE
class IndicatorInterface <TypeEnum::__INPUT> {
    private:
        uint8_t mIndicator_pin;
    public:
        explicit IndicatorInterface(uint8_t mIndicator_pin);
        bool getInputDigital(void);
};
template<>

// For OUTPUT MODE
class IndicatorInterface <TypeEnum :: __OUTPUT> {
     private:
        uint8_t mIndicator_pin;
    public: 
        explicit IndicatorInterface(uint8_t mIndicator_pin);
        bool turnOn (void);
        bool turnOff (void);
};

#endif
