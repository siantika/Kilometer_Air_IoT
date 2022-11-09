#ifndef HEADER_H
#define HEADER_H

/* ID CREDENTIAL */
// Device Phone Number: 082145827382
// provider: Telkomsel

/* ************* IO_Mapping ends here ************* */

// CONSTANTS
#define ID_DEVICE "KMA-01"
#define FIRMWARE_VERSION "1.0 BETA, Copyrights 2022"
#define CORPORATION "VIMANA ELECTRONIC, Siantika"
#define WATER_READ_INTERVAL 1000     // sec
#define ADC_RESOLUTION 1024.0        // 10-byte --> refered to board.
#define V_REF_SERIAL 5.0             // if serial is attached to Device.
#define V_REF_BAT 4.2                // if no serial is attached to Device.
#define ADDR_PHONE 0                 // EEPROM address for phone number
#define ADDR_ALARM_DURATION 20       // EEPROM address for alarm duration
#define SIM800_SLEEP_MODE 2          // src: https://simcom.ee/documents/SIM800/SIM800_Hardware%20Design_V1.09.pdf
#define MAX_CALL_TIME_INTERVAL 20000 // 20 secs is interval from sim800's datasheet
#define FIRST_CALL_TIME_DURATION 0   // just making call in first time, no need to wait the millis function.
#define LED_INTERVAL_200 200         // ms
#define LED_INTERVAL_800 800         // ms
#define LED_INTERVAL_GET_PHONE 800   // ms
#define LED_INTERVAL_SET_ALARM 500   // ms

#endif