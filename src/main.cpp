#include <Arduino.h>
#include <EEPROM.h>
#include "ComInterface.h"
#include "SensorInterface.h"
#include "header.h"
#include "IndicatorInterface.h"

// Debug console
//#define DEBUG // If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.

#ifdef DEBUG                                      // Macros are usually in all capital letters.
#define DPRINT(...) Serial.print(__VA_ARGS__)     // DPRINT is a macro, debug print
#define DPRINTLN(...) Serial.println(__VA_ARGS__) // DPRINTLN is a macro, debug print with new line
#else
#define DPRINT(...)   // now defines a blank line
#define DPRINTLN(...) // now defines a blank line
#endif

/* Global Variables */
volatile bool g_opt_mode;
bool g_sms_flag;
uint8_t g_duration_time;
uint8_t g_state;
bool mLed_state;
int g_alarm_water_threshold = 0;
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
void blinkLedIndicator(uint16_t mLed_interval);
void nextStateFunction(void);

// Setup
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
  g_state = 1;                        // default g_state = 0.

  // load value
  // phone number
  // last of water volume
}

// MAIN FUNCTION
void loop(void)
{
  // set operationMode --> using interrupt button, it changes g_opt_mode (should be volatile)
  uint8_t g_state_prev = g_state;
  Serial.println(g_state_prev);

  switch (g_state)
  {

  // Operation mode 1. Setup a credential.
  case 1:
  {
    // This is initial operation 1 case.
    ledIndicator.turnOn();
    mLed_state = 1; // ON = 1 . OFF = 0
    g_prevTime = 0;

    // call next-state function
    nextStateFunction();
  }
  break;

  /* Get phone number */
  case 2:
  {
    // turn on the  LED Indicator
    ledIndicator.turnOn();
    // get Phone number
     g_phone_number = sim800.getPhone(); // usual code

    // call next-state function
    nextStateFunction();
  }
  break;

  case 3:
  { // STORING and SEND Notify sms
    ledIndicator.turnOff();
    // writeStringToEEPROM(20, g_phone_number);
    String msgs_verif = "Nomor Anda telah teregistrasi...";
    sim800.sendSMS(msgs_verif, g_phone_number);

    nextStateFunction();
  }
  break;

  case 4:
  { // SET ALARM water
    blinkLedIndicator(800);
    g_alarm_water_threshold = 1;
    // g_alarm_water_threshold = atoi(sim800.readSMS().c_str()); // Have to testing !!!!

    nextStateFunction();
  }
  break;

  case 5:
  {

    writeStringToEEPROM(40, String(g_alarm_water_threshold));
    ledIndicator.turnOff();

    nextStateFunction();
  }
  break; // end here for operation mode 1.

  default:
    break;
  }
}

/* Functions */

// next-state function
// It controls the state of main.
void nextStateFunction(void)
{
  switch (g_state)
  {
  case 1:        // setup credential
    g_state = 2; // move to get phone number
    break;

  case 2: // get phone number
    if (g_phone_number.length() > 0 && g_phone_number != "ERROR")
      g_state = 3; // move to store phone number in EEPROM
    else
      g_state = 2; // if it doesn't get phone number, stay in this state.

    break;

  case 3:
    g_state = 4;
    break;

  case 4:
    if (g_alarm_water_threshold == 0)
      g_state = 4; // Water ALARM
    else
      g_state = 5; // ALARM THRESHOLD to eeprom
    break;

  case 5:
    g_state = 6; // begining of operation mode 0
    break;

  default:
    break;
  }
}
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

// Blinking LED Indicator
void blinkLedIndicator(uint16_t mLed_interval)
{
  g_current_time = millis();

  if (g_current_time - g_prevTime >= mLed_interval)
  {
    g_prevTime = g_current_time;
    if (mLed_state == 1)
    {
      ledIndicator.turnOff();
      mLed_state = 0;
    }
    else
    {
      ledIndicator.turnOn();
      mLed_state = 1;
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
