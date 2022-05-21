#include <Arduino.h>
#include "SIM800_COM.h"
#include <EEPROM.h>



SIM800_COM sim800; 
String PHONE="";

// function declaration
void writeStringToEEPROM(int addrOffset, const String &strToWrite);
String readStringFromEEPROM(int addrOffset);
void mainTest(void);

void setup(void) {
  Serial.begin(9600);
   EEPROM.begin();
   sim800.sleepSIM800(2); // SIM800 properties for sleep (TESTING)
}

void loop(void) {
  
 mainTest();
  while(1);
  
}



/* Functions */


// test function --> all testing class/units etc are wrote here

void mainTest(void){
  //   PHONE += sim800.getPhone();
  // // store to eeprom
  // writeStringToEEPROM(ADDR_PHONE, PHONE);
  // // only for checking (PHONE ="")

  PHONE = readStringFromEEPROM(ADDR_PHONE);
  Serial.println(PHONE);
  delay(500);
  String _contentOfmsg = "Selamat , Nomor Telpon Anda "+PHONE+" Telah terdaftar. Silakan ikuti langkah selanjutnya!";
  sim800.sendSMS(_contentOfmsg, PHONE);
  delay(5000); // should be at least 5 secs
  sim800.phoneCall(PHONE);
  delay(10000);
  sim800.hangUpcall();
  delay(1000);
  Serial.println("Done");

}


























// EEPROM. src: https://roboticsbackend.com/arduino-write-string-in-eeprom/

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);

  for (byte i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}


String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];

  for (byte i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; 

  return String(data);
}


