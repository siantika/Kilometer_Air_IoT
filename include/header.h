#ifndef HEADER_H
#define HEADER_H

/* ID CREDENTIAL */
// Device Phone Number: 082145827382
// provider: Telkomsel


/* ************* IO_Mapping ends here ************* */

// CONSTANTS
#define ID_DEVICE "KMA-01"
#define PROGRAM_VERSION "1.0 BETA / 2022"
#define CORPORATION "VIMANA ELECTRONIC"
#define WATER_READ_INTERVAL 1000 // sec
#define ADC_RESOLUTION 1024.0 // 10-byte --> refered to board.
#define V_REF 3.3 // 3.3 V --> refered to board.
#define ADDR_PHONE 0 // EEPROM address for phone number
#define ADDR_ALARM_DURATION 20  // EEPROM address for alarm duration
#define SIM800_SLEEP_MODE 2 // src: https://simcom.ee/documents/SIM800/SIM800_Hardware%20Design_V1.09.pdf

#endif