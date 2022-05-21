/* *********************************************************** */
/* SIM800_COM class                                            */   
/* It handles SIM800 operations                                */ 
/* Created by: I Putu Pawesi Siantika, S.T.                    */
/* Do not forget to increase The buffer memory to 256 bytes    */
/* Src: https://internetofhomethings.com/homethings/?p=927     */
/* *********************************************************** */


#include "SIM800_COM.h"
#include "header.h"

// Enable or disable debug message (#define DEBUG)


#ifdef DEBUG
   #define DEBUG_PRINT replySerial(); // show messages from UART SIM 800 and Arduino
#else
   #define DEBUG_PRINT delay(500); // just add the delay
#endif



SoftwareSerial sim800Serial(SIM_RX, SIM_TX);

// initialization  method
SIM800_COM::SIM800_COM(){
    sim800Serial.begin(BAUDRATE_SIM800);
}

String SIM800_COM::getPhone(){
  _phoneNumber = readSMS();
  return _phoneNumber; 
}


/* Sending a Message */
// @param &messages --> containing message that you want to send 
// @param &phoneNum --> conraining phone number. start with nation code (ex: 62 (IDN))
// please get the phone number first!!!

bool SIM800_COM::sendSMS(String &messages, String &phoneNum){
     
    sim800Serial.println("AT");
    DEBUG_PRINT
    sim800Serial.println("AT+CMGF=1");
    DEBUG_PRINT
    sim800Serial.println("AT+CMGS=\"+"+ phoneNum+ "\"");
    DEBUG_PRINT
    sim800Serial.print(messages); // put content of messages 
    DEBUG_PRINT
    sim800Serial.write(26); // it likes "ctrl+z", sending a message.
    DEBUG_PRINT
    delay(5000); // should put this delay !!!
}


/* !!! Read SMS format : #conten of messages* (#......*) !!!*/
String SIM800_COM::readSMS(){
  sim800Serial.println("AT");
  DEBUG_PRINT
  sim800Serial.println("AT+CMGF=1");
  DEBUG_PRINT
  sim800Serial.println("AT+CNMI=1,2,0,0,0");
  DEBUG_PRINT
  sim800Serial.println();
  DEBUG_PRINT
  serialFlush();
// waiting if there is no data incoming from sim800
  while (sim800Serial.available() == 0);  
    // clear String (fisrt time when code is called)
      _dataIn ="";
      _parseData ="";
      
  // read data from SIM800 uart (while exist)
      while(sim800Serial.available()){
        delay(10);
        char inChar = (char)sim800Serial.read();
        _dataIn += inChar;
      }
      // clear unwanted data
      serialFlush();
  // do parsing when data is available
      if(_dataIn.length() != 0) {
  // parsing data
       for (uint8_t i=0; i<_dataIn.length(); i++){
        if(_dataIn[i] == '#') {
          i++; // to store data after '#'
          while (_dataIn[i] != '*')
          {
            _parseData += _dataIn[i];
            i++;
          }         
        }
      }  
  }
  return _parseData;
}


void SIM800_COM::phoneCall(String &phoneNum){
  sim800Serial.println("AT");
  DEBUG_PRINT
  sim800Serial.println("ATD+" + phoneNum +";");
  DEBUG_PRINT
}

void SIM800_COM::hangUpcall(){
  sim800Serial.println("AT");
  DEBUG_PRINT
  sim800Serial.println("ATH");
  DEBUG_PRINT
}

/* Sleep Mode Function  */
// it only work for SIM800
// Recomended setting: SLEEP_MODE: 0 or 2 (I'll using sleep mode 2 since sleep mode 0 is default mode)
// Not recomended: SLEEP_MODE 1 needs put an extra circuit (pin DTR) ref: https://simcom.ee/documents/SIM800/SIM800_Hardware%20Design_V1.09.pdf (p.26-27)
void SIM800_COM::sleepSIM800(byte SLEEP_MODE){
  sim800Serial.println("AT");
  DEBUG_PRINT
  sim800Serial.println("AT+CSCLK="+ String(SLEEP_MODE));
  delay(500);  // give it time for serial response
  DEBUG_PRINT
  delay(5000);
  serialFlush(); // clear sim800 reply about setting (first time initialization). WARNING: Don't remove This !!!
}


/* Private method */

void SIM800_COM::replySerial(){
  
  while (Serial.available()) 
  {
    sim800Serial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(sim800Serial.available()) 
  {
    Serial.write(sim800Serial.read());//Forward what Software Serial received to Serial Port
  }
  delay(500);  
}

// clear serial buffer in Serial and Sim800 serial
void SIM800_COM::serialFlush(){
  while(Serial.available() > 0) {
     char _t = Serial.read();
  }
  while(sim800Serial.available() > 0){
     char _tSerial = sim800Serial.read();
  }
}