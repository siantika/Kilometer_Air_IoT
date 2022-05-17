/* *************************************
/* Do not forget to increase buffer memory to 256 bytes */
/* Src: https://internetofhomethings.com/homethings/?p=927  */
/* ***************************************             */


#include "SIM800_COM.h"
#include "header.h"



SoftwareSerial sim800Serial(SIM_RX, SIM_TX);


SIM800_COM::SIM800_COM(){
    // initialize software serial for sim800
    sim800Serial.begin(BAUDRATE_SIM800);
    Serial.begin(9600);

}


bool SIM800_COM::sendSMS(String messages){
    _contentOfmsg = messages;
    sim800Serial.println("AT");
    replySerial();
    sim800Serial.println("AT+CMGF=1");
    replySerial();
    sim800Serial.println("AT+CMGS=\"+" + String(_phoneNumber)+"\""); // setup phone number
    replySerial();
    sim800Serial.print(_contentOfmsg); // put content of messages 
    replySerial();
    sim800Serial.write(26); // it likes "ctrl+z", sending a message.
    replySerial();
}


String SIM800_COM::readSMS(){
  // syntax to get sms
  sim800Serial.println("AT");
  replySerial();
  sim800Serial.println("AT+CMGF=1");
  replySerial();
  sim800Serial.println("AT+CNMI=1,2,0,0,0");
  replySerial();
  Serial.println("Kirimkan nomor Anda: ");
  serialFlush();
  serialFlush();
// waiting
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
  // clear UART
      serialFlush();

  // do parsing when data is available
      if(_dataIn.length() != 0) {
  // parsing data
       for (uint8_t i=0; i<_dataIn.length(); i++){

        if(_dataIn[i] == '#') {
          i++; // to store data after '#'
          while (_dataIn[i] != '\0')
          {
            _parseData += _dataIn[i];
            i++;
          }         
        }
      }  
  }
  return _parseData;
}

String SIM800_COM::getPhone(){
  // read sms 
  // pasrsing to get number only
  // Store in EEPROM
  // Load value to Variable
  // Return variable
}






















// Private function
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
  serialFlush();
    
}

String SIM800_COM::parseData(){
  // clear String (fisrt time when code is called)
      _dataIn ="";
      _parseData ="";
      
  // read data from SIM800 uart (while exist)
      while(sim800Serial.available()){
        delay(10);
        char inChar = (char)sim800Serial.read();
        _dataIn += inChar;
      }
  // clear UART
      serialFlush();

  // do parsing when data is available
      if(_dataIn.length() != 0) {
  // parsing data
       for (uint8_t i=0; i<_dataIn.length(); i++){

        if(_dataIn[i] == '#') {
          i++; // to store data after '#'
          while (_dataIn[i] != '\0')
          {
            _parseData += _dataIn[i];
            i++;
          }         
        }
      }  
  }
  return _parseData;
}

void SIM800_COM::serialFlush(){
  while(Serial.available() > 0) {
     char _t = Serial.read();
    
  }
  while(sim800Serial.available() > 0){
     char _tSerial = sim800Serial.read();
  }
}