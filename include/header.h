#ifndef HEADER_H
#define HEADER_H


/* ************* IO_Mapping ************* */ 
// IO_Mapping file --> containing Pins definition

/* LED INDICATOR */
#define PIN_LED_INDICATOR 4 // FOR TESTING. Usual pin is pin 4.

/* BUTTON OPERATION */
#define PIN_BUTTON_OPT 3

/* SIM800 */
#define SIM_RX 10
#define SIM_TX 11

/* Water Flow Sensor's pin */
#define PIN_WATER_FLOW 2 // interrupt number 0

/* Battery Level Sensor's pin */
#define PIN_BATTERY_LEVEL A0

/* ************* IO_Mapping ends here ************* */


// CONST
#define ID_DEVICE "KMA-01" // ID
#define ADDR_PHONE 0 // epprom address to store PHONE NUMBER (interger)
#define TIME_TO_REPORT 15 // hour (am)
#define PHONE_CALL_DELAY 20000 // sec
#define FLOW_TIME_THRESHOLD 6 // sec
#define WARNING_MSG "AIR TERUS MENGALIR! PERIKSA JALUR AIR!!!"
#define WATER_READ_INTERVAL 1000 // sec
#define ADC_RESOLUTION 1024.0
#define V_REF_5V 5.0
#define ADDR_PHONE 0
#define ADDR_ALARM_DURATION 20 

#endif