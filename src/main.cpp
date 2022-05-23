#include <Arduino.h>
#include "SIM800_COM.h"
#include <EEPROM.h>
#include "SensorInterface.h"
#include  "header.h"
#include  "IndicatorInterface.h"

SIM800_COM sim800; 
String PHONE="";

unsigned long currentTime;
unsigned long startTime;
uint8_t durationTime = 0;

WaterFlow WL(PIN_WATER_FLOW); // create an instace of WaterFlow sensor
BatteryLevel batteryLevel(PIN_BATTERY_LEVEL); // create an instace of Battery level sensor
IndicatorInterface <TypeEnum::__OUTPUT> BUZZER(3); // create an instace called Buzzer
IndicatorInterface<TypeEnum::__INPUT> Button_1(4);



// function declaration
void writeStringToEEPROM(int addrOffset, const String &strToWrite);
String readStringFromEEPROM(int addrOffset);
void mainTest_SIM800(void); // test only for SIM800
void mainTest_waterFlow(void); // test only for water flow
void setupWaterflow(void);
void mainTest_BatteryLevel(void);
void mainTest_IndicatorInterface(void);

// SETUP ..
void setup(void) {
  Serial.begin(9600);
   //EEPROM.begin();
   //sim800.sleepSIM800(2); // SIM800 properties for sleep (TESTING)
   //setupWaterflow();
}

// MAIN FUNCTION
void loop(void) {
  mainTest_IndicatorInterface();
  
}



/* Functions */
// test function --> all testing class/units etc are wrote here
void mainTest_SIM800(void){
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


// water flow function test
void setupWaterflow(void){
  WL.init(PIN_WATER_FLOW);
  pinMode(LED_BUILTIN,OUTPUT);
  startTime = millis();
}

void mainTest_waterFlow(void){
   currentTime = millis();
  if (currentTime - startTime > 1000){
    startTime = currentTime;

    Serial.println("Capturing Data: ");
    float dataVolume = WL.getWatervolume();


    // get Duration 
    if (dataVolume > 0.00) durationTime ++;
    else {
      Serial.println(durationTime);
      durationTime = 0; // set to 0
    }

    // set an alarm
    bool alarmState = WL.setVolumealarm(6,durationTime);
if (alarmState == 1) digitalWrite(LED_BUILTIN, HIGH); // turn on LED while condition is reached.
else digitalWrite(LED_BUILTIN,LOW);
  
    Serial.println(dataVolume);
   
  }
}



// Batter Level's Function (TESTING ONLY)

void mainTest_BatteryLevel(void){

  float readBatteryvolt = batteryLevel.getVoltage();
  Serial.println();

}


void mainTest_IndicatorInterface(){
bool readButton = Button_1.getInputdigital();
Serial.println(readButton);
delay(500);
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


