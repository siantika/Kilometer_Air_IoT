/* *********************************************************** */
/* ComInterface class                                          */
/* It handles Communication based on SIMCOM tech (AT COMMANDS) */
/* Created by: I Putu Pawesi Siantika, S.T.                    */
/* Do not forget to increase The buffer memory to 256 bytes    */
/* Src: https://internetofhomethings.com/homethings/?p=927     */
/* *********************************************************** */

#include "ComInterface.h"
#include "header.h"
#include "io_mapping.h"

// Enable or disable serial reply from module (#define DEBUG_COM)
 //#define DEBUG_COM

#ifdef DEBUG_COM
#define DEBUG_PRINT replySerial(); // show messages from UART
#else
#define DEBUG_PRINT // disable replySerial function
#endif

SoftwareSerial comSerial(SIM_RX, SIM_TX);

// constructor
ComInterface::ComInterface()
{
  comSerial.begin(COM_SERIAL_BAUDRATE);
}

int ComInterface::init()
{
  int status_sim = 0;
  /* setup for SMS */
  comSerial.println("AT");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
  serialFlush();
  comSerial.println("AT+CPIN?");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY

  status = replySerial();

  /* checking SIM availability */
  if (status.length() >= ERROR_CPIN)
  {
    status_sim = 0; // SIM is available
  }
  else
  {
    status_sim = 1; // SIM is not available
  }

  comSerial.println("AT+CMGF=1");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
  comSerial.println("AT+CNMI=1,2,0,0,0");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
  comSerial.println();
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
  return status_sim;
}

String ComInterface::getPhone()
{
  mPhone_number = readSMS();
  if (mPhone_number == "ERROR")
    return "ERROR";
  else
    return mPhone_number;
}

/* Sending a Message */
// @param &messages --> containing message that you want to send
// @param &phoneNum --> conraining phone number. start with nation code (ex: 62 (IDN))
// please get the phone number first!!!

void ComInterface::sendSMS(String &messages, String &phoneNum)
{
  comSerial.println("AT");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
  comSerial.println("AT+CMGS=\"+" + phoneNum + "\"");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
  comSerial.print(messages); // put content of messages
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
  comSerial.write(26); // it likes "ctrl+z", enter keyword.
  DEBUG_PRINT
  delay(5000); // should put this delay !!!
}

/* !!! Read SMS format : #conten of messages* (#......*) !!!*/
String ComInterface::readSMS()
{
  // clear String (fisrt time when code is called)
  mData_in = "";
  mParse_data = "";

  // read data from SIM800 uart (while exist)
  while (comSerial.available() > 0)
  {
    delay(10);
    char inChar = (char)comSerial.read();
    mData_in += inChar;
  }
  // clear unwanted data
  serialFlush();
  // do parsing when data is available
  if (mData_in.length() != 0)
  {
    // parsing data
    for (uint8_t i = 0; i < mData_in.length(); i++)
    {
      if (mData_in[i] == '#')
      {
        i++; // to store data after '#'
        while (mData_in[i] != '*')
        {
          mParse_data += mData_in[i];
          i++;

          if (mParse_data.length() > MAX_SMS_CHAR)
          {
            return "ERROR";
            break;
          }
        }
      }
    }
  }
  return mParse_data;
}

void ComInterface::phoneCall(String &phoneNum)
{
  comSerial.println("AT");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
  comSerial.println("ATD+" + phoneNum + ";");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
}

void ComInterface::hangUpcall()
{
  comSerial.println("AT");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
  comSerial.println("ATH");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
}

/* Sleep Mode Function  */
// it only work for SIM800 and SIM900
// Recomended setting: SLEEP_MODE: 0 or 2 (I'll use sleep mode 2 since sleep mode 0 is default mode)
// Not recomended: SLEEP_MODE 1 needs put an extra circuit (pin DTR) ref: https://simcom.ee/documents/SIM800/SIM800_Hardware%20Design_V1.09.pdf (p.26-27)
void ComInterface::sleepMode(byte SLEEP_MODE)
{
  comSerial.println("AT");
  DEBUG_PRINT
  SIM_NORMAL_OPT_DELAY
  comSerial.println("AT+CSCLK=" + String(SLEEP_MODE));
  SIM_NORMAL_OPT_DELAY
  DEBUG_PRINT
  serialFlush(); // clear sim800 reply about setting (first time initialization). WARNING: Don't remove This !!!
}

/* Private method */
String ComInterface::replySerial()
{
  getReply = "";
  while (comSerial.available())
  {
    getReply += (char)comSerial.read();
  }
  #ifdef DEBUG_COM
  Serial.print(getReply);
  #endif
  return getReply;
}

// clear serial buffer in Serial and Sim800 serial
void ComInterface::serialFlush()
{
  while (Serial.available() > 0)
  {
    char _t = Serial.read();
  }
  while (comSerial.available() > 0)
  {
    char _tSerial = comSerial.read();
  }
}
