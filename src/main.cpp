#include <Arduino.h>
#include "SIM800_COM.h"
#include <EEPROM.h>
#include "SensorInterface.h"
#include "header.h"
#include "IndicatorInterface.h"
#include "DS3231.h"

/* Global Variables */
bool g_optMode = 0; // default mode is 0 (Normal Operation)
bool g_SMS_flag;
uint16_t g_timeDuration;
String g_phoneNumber = "";
float g_totalVolume;
String _msgContent;

unsigned long currentTime;
unsigned long prevTime = 0;

uint8_t durationTime = 0;

/* create instances */
SIM800_COM sim800;
WaterFlow WL(PIN_WATER_FLOW);
BatteryLevel batteryLevel(PIN_BATTERY_LEVEL);
IndicatorInterface<TypeEnum::__OUTPUT> BUZZER(3);
IndicatorInterface<TypeEnum::__INPUT> Button_1(4);
DS3231 rtc(SDA, SCL);
Time t;

/* forward functions declaration */
void writeStringToEEPROM(int addrOffset, const String &strToWrite);
String readStringFromEEPROM(int addrOffset);
void mainTest_SIM800(void);    // test only for SIM800
void mainTest_waterFlow(void); // test only for water flow
void setupWaterflow(void);
void mainTest_BatteryLevel(void);
void mainTest_IndicatorInterface(void);
// get Hour func
bool dailySendreport(uint8_t _hour, uint8_t _minute, uint8_t _sec);
void calculateVolume_timeDuration(void);

// SETUP ..
void setup(void)
{
  Serial.begin(9600);
  EEPROM.begin();
  sim800.sleepSIM800((byte)2); // SIM800 properties for sleep (TESTING)
  setupWaterflow();
  rtc.begin();
}

// MAIN FUNCTION
void loop(void)
{
  // set operationMode
  if (g_optMode == 0)
  {
    // get hour

    // Testing Only
    //Serial.println(dailySendreport());
    g_SMS_flag = dailySendreport(16,38,40); // hh/mm/ss

    if (g_SMS_flag)
    {
      //  do: read Battery
      Serial.println("baca baterai...");

      //  send a message contained: battery level in V and total volume
      _msgContent = "Selamat pagi, berikut informasi dari  " + String(ID_DEVICE) + " : Volume air dipakai = " + String(g_totalVolume) + ". Tegangan Baterai: "; // fill this with battery var.
      sim800.sendSMS(_msgContent, g_phoneNumber);
      g_SMS_flag = 0; 
    }

    // read water flow every 1 sec
    calculateVolume_timeDuration();
    // set alarm
    bool alarmState = WL.setVolumealarm(FLOW_TIME_THRESHOLD, durationTime);
    if (alarmState == 1)
    {
      sim800.phoneCall(g_phoneNumber);
      delay(PHONE_CALL_DELAY);
      // _msgContent = WARNING_MSG;
      sim800.sendSMS(_msgContent, g_phoneNumber);
    }
  }

  // setup credential
  //  else if g_optMode == 1
  //      do
}

/* Functions */

bool dailySendreport(uint8_t _hour, uint8_t _minute, uint8_t _sec)
{
  // alarm to execute
  t = rtc.getTime();
  uint8_t _hourNow = t.hour;
  uint8_t _minuteNow = t.min;
  uint8_t _secNow = t.sec;
  return (_hour == _hourNow && _minute == _minuteNow && _sec == _secNow);
}

// dailyTask every 1 sec
void calculateVolume_timeDuration(void)
{
  currentTime = millis();
  if (currentTime - prevTime >= WATER_READ_INTERVAL)
  {
    prevTime = currentTime;
    // convert
    float dataVolume = WL.getWatervolume();

    g_totalVolume += dataVolume;
    Serial.print(" total volume air: ");
    Serial.println(g_totalVolume);
    Serial.print(" Debit air: ");
    Serial.println(dataVolume);
    // get time duration when water is flowing
    if (dataVolume > 0.00)
      durationTime++;
    else
    {
      Serial.println(durationTime);
      durationTime = 0; // reset duration time
    }
  }
}

// test function --> all testing class/units etc are wrote here

// water flow function test
void setupWaterflow(void)
{
  WL.init(PIN_WATER_FLOW);
  pinMode(LED_BUILTIN, OUTPUT);
}

// Batter Level's Function (TESTING ONLY)

void mainTest_BatteryLevel(void)
{

  float readBatteryvolt = batteryLevel.getVoltage();
  Serial.println();
}

void mainTest_IndicatorInterface()
{
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
