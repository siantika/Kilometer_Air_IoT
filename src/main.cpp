/***************************************************************************
 * Created by: I Putu Pawesi Siantika. email: csiantka@gmail.com. Jun 2022 *
 * Kilometer Air IoT Version 1.0 (BETA)                                    *
 * Monitoring water volume and alarming when leakage ocurred               *
 * This is Private Code.                                                   *
 * Thankyou ...                                                            *
 ***************************************************************************
 */

#include <Arduino.h>
#include <EEPROM.h>
#include "ComInterface.h"
#include "SensorInterface.h"
#include "header.h"
#include "IndicatorInterface.h"
#include "io_mapping.h"

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
uint8_t g_duration_time;
uint8_t g_state;
bool g_opt_mode;
bool g_serial_is_read;
bool g_call_status;
bool mLed_state;
bool g_flag_phone_num_to_run, g_flag_alarm_duration_to_run;
uint16_t g_alarm_water_threshold;
uint16_t g_call_time_interval;
int g_status_sim; 
float g_total_volume;
float g_water_flow;
String g_phone_number;
String g_msg_content;
String g_get_command_sms;
unsigned long g_current_time_read_water;
unsigned long g_prev_time_read_water;
unsigned long g_current_time_call;
unsigned long g_prev_time_call;
unsigned long g_prev_time_led;
unsigned long g_current_time_led;

/* Constants */
const String MSG_NOTIFY_REG = "Nomor Anda telah teregistrasi. Silakan atur waktu alarm (dalam detik, Maks. 64000 detik)";
const float CALLIBRATION_KWA = 0.48; // (liter) Self callibration (Instrumentation: Meteran Air Plastik AMB, loc: Bali, IDN, 4 Jun 2022)

/* Creating instances */
ComInterface sim800;
WaterFlow waterFlow{PIN_WATER_FLOW};
IndicatorInterface<TypeEnum::__INPUT> buttonOpt{PIN_BUTTON_OPT}; // using curly bracket because it prevents constructor casting parameter (if you use (), it does casting)
IndicatorInterface<TypeEnum::__OUTPUT> ledIndicator{PIN_LED_INDICATOR};

/* forward functions declaration */
String readStringFromEEPROM(int addrOffset);
void writeStringToEEPROM(int addrOffset, const String &strToWrite);
void readWaterVolumeAndWaterflowDuration(void);
void blinkLedIndicator(uint16_t mLed_interval);
void nextStateFunction_opt1(void);
void permitToMainCode(void);
void initState(void);
void getPhoneNumber(void);
void storeAndNotifySms(int eeprom_address, String data_to_write_in_eeprom, char msg_type);
void setAndGetWaterAlarmDuration(void);
void nextStateFunction_opt0(void);
void callUserInPeriodicTime(void);
void getComandFromSms(void);
void handlingCommandFromSms(void);
void checkingOperationMode(void);
void hangUpCall(void);
void showFirmwareVersion(void);

/* Setup */
void setup(void)
{

  delay(3000); // Minimum is 3 secs (Initialization for SIM800L. Check Datasheet: https://www.filipeflop.com/img/files/download/Datasheet_SIM800L.pdf / p.24)
  Serial.begin(9600);
  g_status_sim = sim800.init();
  
  /* no SIM Card inserted, blink the led and stuck*/
  if (g_status_sim == 1)
  {
    while (1)
    {
      blinkLedIndicator(0);
    }
  }

  sim800.sleepSIM800(SIM800_SLEEP_MODE);
  waterFlow.init();

  /* initial value */
  g_serial_is_read = 0;
  g_phone_number = "";
  g_call_status = 0;
  g_alarm_water_threshold = 0;
  g_prev_time_read_water = 0;
  g_duration_time = 0;
  g_total_volume += CALLIBRATION_KWA;
  g_state = 1; // make it jumps to case 1 (default)
  g_call_time_interval = FIRST_CALL_TIME_DURATION;

  /* detect operation mode (Mode )
    LOW (0)  = operation 0 --> monitor the water flow
    HIGH (1) = operation 1 --> setup a credential
  */
  g_opt_mode = buttonOpt.getInputDigital();

  /* prerequisite check */
  permitToMainCode();
}

// Driver code
void loop(void)
{
  // show software version when serial available (once)
  showFirmwareVersion();
  if (g_opt_mode == 0)
  {

    switch (g_state)
    {
      /* Operation mode 0. monitoring water volume and flow.
        + @param (String) g_get_command_sms .
        + input parameter using sms from user through sim800.serial() in comInterface lib.
      */

    case 1:
    {
      readWaterVolumeAndWaterflowDuration();
      nextStateFunction_opt0();
    }
    break;

    case 2:
    {
      
      callUserInPeriodicTime();
      nextStateFunction_opt0();
    }
    break;

    case 3:
    {
      getComandFromSms();
      nextStateFunction_opt0();
    }
    break;

    case 4:
    {
      handlingCommandFromSms();
      nextStateFunction_opt0();
    }
    break;
    case 5:
    {
      hangUpCall();
      nextStateFunction_opt0();
    }

    default:
      break;
    }
  }

  else if (g_opt_mode == 1)
  {
    /* Operation mode 1. Setup a credential.
      + @param (String) g_phone_g_phone_number, (uint16_t) g_alarm_water_threshold .
      + input parameter using sms from user through sim800.serial() in comInterface lib.
    */
    switch (g_state)
    {

    case 1:
    {
      initState();
      nextStateFunction_opt1();
    }
    break;

    case 2:
    {
      getPhoneNumber();
      nextStateFunction_opt1();
    }
    break;

    case 3:
    {
      storeAndNotifySms(ADDR_PHONE, g_phone_number, 'P');
      nextStateFunction_opt1();
    }
    break;

    case 4:
    {
      setAndGetWaterAlarmDuration();
      nextStateFunction_opt1();
    }
    break;

    case 5:
    {
      storeAndNotifySms(ADDR_ALARM_DURATION, String(g_alarm_water_threshold), 'A');
      nextStateFunction_opt1();
    }
    break;

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
}

/* *************************************** -- ************************************ */
/* Functions */

/* checking resources function */

void permitToMainCode(void)
{
  // stored phone number
  if (readStringFromEEPROM(ADDR_PHONE) != 0 && readStringFromEEPROM(ADDR_ALARM_DURATION) != 0)
  {
    g_phone_number = readStringFromEEPROM(ADDR_PHONE);
    g_flag_phone_num_to_run = 1;
    DPRINT(F("Phone number is: "));
    DPRINTLN(g_phone_number);
  }
  else
  {
    g_flag_phone_num_to_run = 0;
    DPRINTLN(F("No phone number"));
  }

  // stored alarm time duration
  if (readStringFromEEPROM(ADDR_ALARM_DURATION) != 0)
  {
    g_alarm_water_threshold = atoi(readStringFromEEPROM(ADDR_ALARM_DURATION).c_str());
    g_flag_alarm_duration_to_run = 1;
    DPRINT(F("Alarm duration is: "));
    DPRINTLN(g_alarm_water_threshold);
  }
  else
  {
    g_flag_alarm_duration_to_run = 0;
    DPRINTLN(F("No alarm duration"));
  }
    
  
  if (g_flag_alarm_duration_to_run * g_flag_phone_num_to_run != 1)
  {

    while (1) 
      blinkLedIndicator(LED_INTERVAL_200);
  }
}

/* event functions */
// Operation Mode 0

void readWaterVolumeAndWaterflowDuration(void)
{
  // It gets water volume and water flow. its store in g_total_volume and g_water_flow(private)
  // it gets time duration and store it in g_duration_time
  g_current_time_read_water = millis();

  if (g_current_time_read_water - g_prev_time_read_water >= WATER_READ_INTERVAL)
  {
    g_prev_time_read_water = g_current_time_read_water;
    g_water_flow = waterFlow.getWaterVolume();
    g_total_volume += g_water_flow;

    DPRINT(F("Total water volume (l): "));
    DPRINTLN(g_total_volume);
    DPRINT(F("Water flow (l/s): "));
    DPRINTLN(g_water_flow);

    // get time duration when water is flowing
    if (g_water_flow > 0.00)
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

void callUserInPeriodicTime(void)
{
  blinkLedIndicator(LED_INTERVAL_200);
  g_current_time_call = millis();
  if (g_current_time_call - g_prev_time_call >= g_call_time_interval)
  {
    g_call_status = 1; // it is happening for calling
    g_call_time_interval = MAX_CALL_TIME_INTERVAL;
    g_prev_time_call = g_current_time_call;
    sim800.phoneCall(g_phone_number);
    DPRINTLN(F("Calling User..."));
  }
}

void getComandFromSms(void)
{
  ledIndicator.turnOff();
  g_call_time_interval = FIRST_CALL_TIME_DURATION;
  g_get_command_sms = sim800.readSMS();
  g_get_command_sms.toLowerCase();
}

void handlingCommandFromSms(void)
{
  ledIndicator.turnOn();
  DPRINT(F("Water volume is "));
  DPRINT(g_total_volume);
  DPRINTLN(F(" Liters"));

  g_msg_content = "*** INFO PERANGKAT ***\n 1. Volume air terpakai " + String(g_total_volume) + " L.\n 2. Durasi alarm " + String(g_alarm_water_threshold) + " dtk.";
  sim800.sendSMS(g_msg_content, g_phone_number);
  ledIndicator.turnOff();
}

void hangUpCall(void)
{
  sim800.hangUpcall();
  g_call_status = 0; // reset it
}

// Operation mode 1
void initState(void)
{
  mLed_state = 1; // ON = 1 . OFF = 0
  g_prev_time_read_water = 0;
}

void getPhoneNumber(void)
{
  blinkLedIndicator(LED_INTERVAL_GET_PHONE);
  g_phone_number = sim800.getPhone();
}

void storeAndNotifySms(int eeprom_address, String data_to_write_in_eeprom, char msg_type)
{
  String mMsg_content = "";
  ledIndicator.turnOff();
  writeStringToEEPROM(eeprom_address, data_to_write_in_eeprom);
  if (msg_type == 'P') // Phone message notify
    mMsg_content = MSG_NOTIFY_REG;
  else if (msg_type == 'A')                                                                                                                          // Alarm message notify                                                                                                                        // alarm message notify
    mMsg_content = "Alarm diatur di " + String(data_to_write_in_eeprom) + " detik. Silakan pindahkan tuas ke mode 0 dan hidupkan ulang daya alat !"; // HERE is a content of message sent to user
  sim800.sendSMS(mMsg_content, g_phone_number);
}

void setAndGetWaterAlarmDuration(void)
{
  blinkLedIndicator(LED_INTERVAL_SET_ALARM);
  g_alarm_water_threshold = atoi(sim800.readSMS().c_str());
}

/* State function for operation 0 */
// It controls the state of operation mode 0 ()
void nextStateFunction_opt0(void)
{
  switch (g_state)
  {
  case 1:
  {
    if (g_duration_time >= g_alarm_water_threshold)
    {
      g_state = 2; // move to callUserInPeriodicTime
    }
    else if (g_duration_time == 0 && g_call_status == 1)
    {
      g_state = 5; // Hang up the call
    }
    else if (g_duration_time == 0 && g_call_status == 0)
    {
      g_state = 3; // move to getComandFromSms
    }
  }
  break;

  case 2:
  {
    g_state = 1; // move to readWaterVolumeAndWaterflowDuration
  }
  break;

  case 3:
  {
    if (g_get_command_sms == "check")
    {
      g_state = 4; // move to handlingCommandFromSms
    }
    else
    {
      g_state = 1; // move to readWaterVolumeAndWaterflowDuration
    }
  }
  break;
  case 4:
  {
    g_state = 1; // move to readWaterVolumeAndWaterflowDuration.
  }
  break;
  case 5:
  {
    g_state = 1; // move to readWaterVolumeAndWaterflowDuration.
  }
  break;

  default:
    break;
  }
}

/* next-state function opt_mode_1 */
// It controls the state of opt_mode_1.
void nextStateFunction_opt1(void)
{
  switch (g_state)
  {
  /* setup credential */
  case 1:
  {
    g_state = 2; // move to getPhoneNumber
    break;
  }

    /* get a phone number */
  case 2:
  {
    if (g_phone_number.length() > 0 && g_phone_number != "ERROR")
      g_state = 3; // move to storeAndNotifySms for phone number
    else
      g_state = 2; // stay in this state.
  }
  break;

    /* Store and Notify SMS */
  case 3:
  {
    g_state = 4; // move to setAndGetWaterAlarmDuration
  }
  break;

    /* set and get water alarm duration */
  case 4:
  {
    if (g_alarm_water_threshold == 0)
      g_state = 4; // stay in this state
    else
      g_state = 5; // move to storeAndNotifySms
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

// Blinking LED Indicator
void blinkLedIndicator(uint16_t mLed_interval)
{
  g_current_time_led = millis();

  if (g_current_time_led - g_prev_time_led >= mLed_interval)
  {
    g_prev_time_led = g_current_time_led;
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

void showFirmwareVersion(void)
{
  if (Serial && g_serial_is_read == 0)
  {
    // set Voltage reference to 5 V
     ;
    Serial.println(F(" * ------------------- Kilometer Air ------------------ * "));
    Serial.println(" Firmware Version: " + String(FIRMWARE_VERSION));
    Serial.println(" ID Device       : " + String(ID_DEVICE));
    Serial.println(" Corporation     : " + String(CORPORATION));
    Serial.println(F(" * ----------------------- *** ----------------------- * "));
    Serial.println(F(" Device info: "));    Serial.println(" * Alarm trigger duration: " + String(g_alarm_water_threshold) + " Secs");
    Serial.println(" * Phone registered      : " + g_phone_number);
    Serial.flush();
    g_serial_is_read = 1;
  }

  if (!Serial)
  {
    g_serial_is_read = 0;
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
