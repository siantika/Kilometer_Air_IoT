#ifndef COMINTERFACE_H
#define COMINTERFACE_H
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "header.h"

#define BAUDRATE_SIM800 9600 // bps
#define ERROR_CPIN 17 // error length in AT+CPIN's response (no sim card)

#define MAX_SMS_CHAR 15

class ComInterface
{
private:
  String getReply = "";
  String status = "";
  String mPhone_number;
  String mMsg;
  String mContent_of_msg;
  String mData_in;
  String mParse_data;
  String replySerial(void);
  void serialFlush();

public:
  ComInterface();
  int init(void);
  String readSMS();
  String getPhone();
  void sendSMS(String &messages, String &phoneNum);
  void sleepSIM800(byte sleep_mode);
  void hangUpcall(void);
  void phoneCall(String &phoneNum);
};

#endif