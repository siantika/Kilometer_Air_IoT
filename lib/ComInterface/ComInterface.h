#ifndef COMINTERFACE_H
#define COMINTERFACE_H
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "header.h"

#define BAUDRATE_SIM800 9600 //bps

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
        bool sendSMS(String &messages, String &phoneNum);
        bool checkSimSerial();
        void sleepSIM800(byte sleep_mode);
        void replySerial(void);
        void phoneCall(String &phoneNum);
        void hangUpcall(void);
           
                
};

#endif