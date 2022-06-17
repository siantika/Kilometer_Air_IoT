#include <Arduino.h>
#include <EEPROM.h>
#include "ComInterface.h"
#include "SensorInterface.h"
#include "header.h"
#include "IndicatorInterface.h"

// Debug console
#define DEBUG //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.

#ifdef DEBUG                                      //Macros are usually in all capital letters.
#define DPRINT(...) Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
#define DPRINTLN(...) Serial.println(__VA_ARGS__) //DPRINTLN is a macro, debug print with new line
#else
#define DPRINT(...)   //now defines a blank line
#define DPRINTLN(...) //now defines a blank line
#endif

/* Global Variables */
volatile bool g_opt_mode;
bool g_sms_flag;
uint8_t g_duration_time;
float g_total_volume;
const float CALLIBRATION_KWA = 48E-5; // Self callibration (Instrumentation: Meteran Air Plastik AMB, loc: Bali, IDN, 4 Jun 2022)
String g_phone_number;
String g_msg_content;
unsigned long g_current_time;
unsigned long g_prevTime;

/* Creating instances */
ComInterface sim800;
WaterFlow waterFlow(PIN_WATER_FLOW);
BatteryLevel batteryLevel(PIN_BATTERY_LEVEL);
IndicatorInterface<TypeEnum::__INPUT> buttonOpt(PIN_BUTTON_OPT); // fixing ... !!!!!!!!!!!!!!!!!!!
IndicatorInterface<TypeEnum::__OUTPUT> ledIndicator(PIN_LED_INDICATOR);

/* forward functions declaration */
String readStringFromEEPROM(int addrOffset);
void writeStringToEEPROM(int addrOffset, const String &strToWrite);
bool dailySendReport(uint8_t hour, uint8_t minute, uint8_t sec);
void periodicTasks(void);

// SETUP ..
void setup(void)
{
  Serial.begin(9600);
  EEPROM.begin();
  sim800.sleepSIM800(2); // Sleep mode 2 (src: https://simcom.ee/documents/SIM800/SIM800_Hardware%20Design_V1.09.pdf)
  waterFlow.init();

  // initial value
  g_opt_mode = 0;
  g_phone_number = "";
  g_prevTime = 0;
  g_duration_time = 0;
  g_total_volume += CALLIBRATION_KWA; // add callibration const
}

// MAIN FUNCTION
void loop(void)
{

  // set operationMode --> using interrupt button, it changes g_opt_mode (should be volatile)
  
  if (g_opt_mode == 0)
  {

    // set alarm for sending daily report once. Set it on 7 am everyday

    if (g_sms_flag)
    {
      //  do: read Battery
      float _batLevel = batteryLevel.getVoltage();
      //  send a message contained: battery level in V and total volume in m3
      g_msg_content = "Selamat pagi, berikut informasi dari  " + String(ID_DEVICE) + " : Volume air dipakai = " + String(g_total_volume) + "m3. Tegangan Baterai: " + String(_batLevel) + " V"; // fill this with battery var.
      sim800.sendSMS(g_msg_content, g_phone_number);
      g_sms_flag = 0; // disable dailyReport sms
    }

    // run periodicTasks function (every 1 sec)
    periodicTasks();

    // set and check alarm for water leaking
    bool mAlarm_state = waterFlow.setVolumeAlarm(FLOW_TIME_THRESHOLD, g_duration_time);

    if (mAlarm_state)
    {
      sim800.phoneCall(g_phone_number);
      delay(PHONE_CALL_DELAY);
      g_msg_content = WARNING_MSG;
      sim800.sendSMS(g_msg_content, g_phone_number);
    }
  }

  // setup credential
  else if (g_opt_mode == 1)
  {
  }
}

/* Functions */

/* periodic tasks */
// It gets water volume and water flow. its store in g_total_volume and mWater_flow(private)
// it gets time duration and store it in g_duration_time
void periodicTasks(void)
{
  g_current_time = millis();

  if (g_current_time - g_prevTime >= WATER_READ_INTERVAL)
  {
    g_prevTime = g_current_time;

    float mWater_flow = waterFlow.getWaterVolume();

    g_total_volume += mWater_flow;

    DPRINT(F("Total water volume (m3): "));
    DPRINTLN(g_total_volume);
    DPRINT(F("Water flow (m3/s): "));
    DPRINTLN(mWater_flow);

    // get time duration when water is flowing
    if (mWater_flow > 0.00)
      g_duration_time++;
    else
    {
      DPRINT(F("Water was flowing in : "));
      DPRINT(g_duration_time);
      DPRINTLN(F(" seconds"));
      DPRINTLN(F("-------------------------------------------\n"));

      g_duration_time = 0; // reset duration time
    }
  }
}

// EEPROM. src: https://roboticsbackend.com/arduino-write-string-in-eeprom/
void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte mLen = strToWrite.length();
  EEPROM.write(addrOffset, mLen);

  for (byte i = 0; i < mLen; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(int addrOffset)
{
  int mNewStrLen = EEPROM.read(addrOffset);
  char mData[mNewStrLen + 1];

  for (byte i = 0; i < mNewStrLen; i++)
  {
    mData[i] = EEPROM.read(addrOffset + 1 + i);
  }
  mData[mNewStrLen] = '\0';

  return String(mData);
}
