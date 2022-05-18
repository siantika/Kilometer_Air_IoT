
#include <Arduino.h>
#include "SIM800_COM.h"
#include "SoftwareSerial.h"


SIM800_COM sim800; 
String PHONE;


void setup() {
  Serial.begin(9600);
  Serial.println("Insert your phone number (send the message) :");
  PHONE =  sim800.getPhone();
  Serial.println(PHONE);
  delay(1000);
  sim800.sendSMS("Hai Ini adalah IoT kwh meter ,,,", PHONE);
 
  
}

void loop() {
  
  
 
  
}
