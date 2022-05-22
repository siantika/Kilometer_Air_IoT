#ifndef SIM800C_COM_H
#define SIM800_COM_H
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "header.h"

#define BAUDRATE_SIM800 9600 //bps

class SIM800_COM {
     private:
        String _phoneNumber;
        String _messages;
        String _contentOfmsg;
        String parseData();
        void serialFlush();

      public:
        SIM800_COM();
        String readSMS();
        String getPhone();
        String _dataIn;  
        String _parseData ;
        bool sendSMS(String &messages, String &phoneNum);
        void sleepSIM800(byte SLEEP_MODE);
        void replySerial(void);
        void phoneCall(String &phoneNum);
        void hangUpcall(void);
           
                
};

#endif