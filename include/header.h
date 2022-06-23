#ifndef HEADER_H
#define HEADER_H

/* ID CREDENTIAL */
// Device Phone Number: 082145827382
// provider: Telkomsel



/* ************* IO_Mapping ends here ************* */


// CONSTANTS
#define ID_DEVICE "KMA-01"
#define PHONE_CALL_DELAY 20000 // sec
#define WARNING_MSG "AIR TERUS MENGALIR! PERIKSA JALUR AIR!!!"
#define WATER_READ_INTERVAL 1000 // sec
#define ADC_RESOLUTION 1024.0 // 10-byte
#define V_REF 3.3
#define ADDR_PHONE 0 // EEPROM address for phone number
#define ADDR_ALARM_DURATION 20  // EEPROM address for alarm duration
#define SIM800_SLEEP_MODE 2 // src: https://simcom.ee/documents/SIM800/SIM800_Hardware%20Design_V1.09.pdf

#endif