#ifndef SIM800C_COM_H
#define SIM800_COM_H
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "header.h"

#define BAUDRATE_SIM800 9600

class SIM800_COM {
    public:
        SIM800_COM(char* PHONE_NUMBER);
        bool sendSMS(char* messages);
        bool phoneCall();
};

#endif