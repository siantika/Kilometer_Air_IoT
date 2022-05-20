#ifndef SIM800C_COM_H
#define SIM800_COM_H
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "header.h"

#define BAUDRATE_SIM800 9600
#define DELAY_WAITING_CALL 15000 // secs



class SIM800_COM {
    public:
        SIM800_COM();
        bool sendSMS(String messages, String phoneNum);
        String readSMS();
        String getPhone();
        String _dataIn;  
        String _parseData ;
        bool sleepSIM800();
        bool wakeUpSIM800();
        void replySerial(void);
        void phoneCall(String &phoneNum);
        void hangUpcall(void);
           
        

    private:
        String _phoneNumber;
        String _messages;
        String _contentOfmsg;
        void serialFlush();
        String parseData();
                
};

#endif