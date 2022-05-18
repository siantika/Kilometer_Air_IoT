#ifndef SIM800C_COM_H
#define SIM800_COM_H
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "header.h"

#define BAUDRATE_SIM800 115200



class SIM800_COM {
    public:
        SIM800_COM();
        bool sendSMS(String messages, String phoneNum);
        bool phoneCall();
        String readSMS();
        String getPhone();
        String _dataIn;  
        String _parseData ;
        bool sleepSIM800();
        bool wakeUpSIM800();
        void replySerial();
           
        

    private:
        String _phoneNumber;
        String _messages;
        String _contentOfmsg;
        void serialFlush();
        String parseData();
        
        
        
};

#endif