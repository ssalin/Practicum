/*
 * BluDaq Firmware
 * Macro Definitions File
 * Nov 2016
 * 
 */

//
//  Libraries and Refrences:
//
#include <Arduino.h>  // Arduino Type Definitions





//
// Debug and Option Flags:
//

//#define DEBUG_EN   // Disable Auth
#define SERIAL_EN  // Enable Serial Debug Statements



//
// Defined Constants:
//

#define NUM_RELAY 2           // Number of Relays
//#define BAUD_RATE 115200      // Better Baud Rate for Serial
#define BAUD_RATE 9600        // Typical Baud Rate for Serial


//
// ATMega328/P Pin Definitions:
//

#define BT_STS 3        // BT Status Pin 
#define PIR0 4          // PIR motion sensor (active low)
#define R0_SET 5        // Set Relay 0
#define R0_RSET 6       // Reset Relay 0
#define R1_SET 7        // Set Relay 1
#define R1_RSET 8       // Reset Relay 1
#define LIGHT_SENSOR 0  // Analog 5v input



//
// Data Structures:
//

// Auth Data - For validating the host
typedef struct{
  bool auth_set;    // Authentication Paired
  int touch;        // Last-updated (date/time)
  double key;       // Auth-key
  double poll_freq; // Polling Frequency (power management)
} auth_data;

// Sensor Data - Stores "current" values
typedef struct{

  int ls;               // Light Sensor Analog Value (0-1023)
  bool PIR;             // Motion Sensor State
  float temp;           // Temperature
  float pressure;       // Pressure
  float humidity;       // Humidity
} sensor_data;

// Automation - Settings for Relay automation
typedef struct {

  // Relay 0 Automation Flags (1 byte):
  byte en   : 1;   // Enable
  byte dec  : 1;   // Descending set point
  byte tog  : 1;   // Toggle Relay
  byte tmp  : 1;   // Temperature Sensor
  byte pres : 1;   // Pressure Sensor
  byte hum  : 1;   // Humidity sensor
  byte ls   : 1;   // Light sensor
  byte pir  : 1;   // PIR Motion Sensor

  float setpoint_0;   // Setpoint 
  int t_duration_0;   // Toggle Duration
} auto_data;
 


//
// EEPROM Settings:
//

#define EEPROM_SIZE 1024;      // Size of EEPROM in Bytes
#define AUTH_ADDR 0            // Address of Authentication Structure
#define AUTO_ADDR 12           // Address of Automation Structure(s)
#define AUTO_SIZE 7            // Size of Automation Struct, 7 per struct, 14 for both structs
#define AUTH_SIZE 11           // Size of Authentication Struct
                               // Size of sensor data is 15

