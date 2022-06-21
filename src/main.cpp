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
uint16_t g_alarm_water_threshold = 0; // hot fix test id 014
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
  sim800.init();
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

  /* Initial */
  case 1:
  {
    // This is initial operation 1 case.
    mLed_state = 1; // ON = 1 . OFF = 0
    g_prevTime = 0;

    nextStateFunction();
  }
  break;

  /* Get phone number */
  case 2:
  {
    blinkLedIndicator(500);             // blink the  LED Indicator
    g_phone_number = sim800.getPhone(); // get Phone number

    nextStateFunction();
  }
  break;

    /* Store and Notify SMS */
  case 3:
  {
    ledIndicator.turnOff();
    writeStringToEEPROM(ADDR_PHONE, g_phone_number);
    String msgs_verif = "Nomor Anda telah teregistrasi. Silakan atur waktu alarm (dalam detik, Maks 64000 detik)";
    sim800.sendSMS(msgs_verif, g_phone_number);

    nextStateFunction();
  }
  break;

    /* set and get water alarm duration  */
  case 4:
  {
    blinkLedIndicator(800);
    g_alarm_water_threshold = atoi(sim800.readSMS().c_str()); // casting from String to interger

    nextStateFunction();
  }
  break;

    /* Store and Notify Alarm Duration */
  case 5:
  {
    writeStringToEEPROM(ADDR_ALARM_DURATION, String(g_alarm_water_threshold));
    ledIndicator.turnOff();
    String msg_duration_alarm_verify = "Alarm diatur di " + String(g_alarm_water_threshold) + " detik. Silakan pindahkan tuas ke mode 0 dan hidupkan ulang daya alat !";
    sim800.sendSMS(msg_duration_alarm_verify, g_phone_number);

    nextStateFunction();
  }
  break; // end here for operation mode 1.

  case 6:
  {
    // idle
    // device should be restarted and move the operation switch to mode 0 !
  }
  break;

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
  /* setup credential */
  case 1:
  {
    g_state = 2; // move to get phone number
    break;
  }

 /* get a phone number */ 
  case 2: 
  {
    if (g_phone_number.length() > 0 && g_phone_number != "ERROR")
      g_state = 3; // move to Store and Notify SMS
    else
      g_state = 2; // stay in this state.
  }
    break;

 /* Store and Notify SMS */
  case 3:
  {
    g_state = 4; // move to set and get water alarm duration
  }
    break;

/* set and get water alarm duration */
  case 4:
  {
    if (g_alarm_water_threshold == 0)
      g_state = 4; // stay in this state
    else
      g_state = 5; // move to Store and Notify Alarm Duration
  }
    break;

/* Store and Notify Alarm Duration */
  case 5:
  {
    g_state = 6; // Idle
  }
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
  EEPROM.update(addrOffset, mLen);

  for (byte i = 0; i < mLen; i++)
  {
    EEPROM.update(addrOffset + 1 + i, strToWrite[i]);
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
