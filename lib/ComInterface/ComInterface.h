#ifndef COMINTERFACE_H
#define COMINTERFACE_H
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "header.h"

#define BAUDRATE_SIM800 9600 //bps

#define MAX_SMS_CHAR 15 

class ComInterface {
     private:
        String mPhone_number;
        String mMsg;
        String mContent_of_msg;
        String parseData();
        void serialFlush();

      public:
        ComInterface();
        String readSMS();
        String getPhone();
        String mData_in;  
        String mParse_data ;        
        bool checkSimSerial();
        void sendSMS(String &messages, String &phoneNum);
        void init(void);
        void sleepSIM800(byte sleep_mode);
        void replySerial(void);
        void phoneCall(String &phoneNum);
        void hangUpcall(void);
           
                
};

#endif