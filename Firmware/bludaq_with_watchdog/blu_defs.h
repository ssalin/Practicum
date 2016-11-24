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
  byte key;       // Auth-key
  byte poll_freq; // Polling Frequency (power management)
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

//
// Serial Communication - Message Stack
// Message Structure:  <Send Type> = <Body Type> or <Value>
//


// Send Message Types: 

#define M_BADMSG "UNKO="   // Bad Message = <Incoming Message>
#define M_HELLO  "HELO="   // Hello = <UUID?>
#define M_AUTH   "AUTH="   // Authentication Status = <T/F>
#define M_SLEEP  "SLEP="  // Device will sleep = <Duration>
#define M_ERROR  "EROR="   // Device Error = <Error Message or ID>
#define M_TEMP   "DTMP"    // Temperature = <Value>
#define M_HUMID  "DHUM"    // Humidity = <Value>
#define M_PRESS  "DPRS"    // Pressure = <Value>
#define M_PHOTO  "DRLS"    // Photoresistor = <Value>
#define M_PIR    "DPIR"    // Motion Sensor = <T/F>
#define M_RL0    "DRL0"    // Relay 0 State = <T/F> (on / off)
#define M_RL1    "DRL1"    // Relay 1 State = <T/F> (on / off)


// Incoming Message Types: <Recieve Type> = <Body Type> or <Value>
#define RM_AUTOCHAN  "AUTN="   // Automation Channel Select = <Channel>
#define RM_AUTOFLAG  "AUTF="   // Automation Flag Set = <Byte>
#define RM_AUTOSET   "AUTS="   // Automation Setupoint = <Float> (4-byte)
#define RM_AUTODUR   "AUTD="   // Automation Duration = <Int> (2-byte)
#define RM_AUTOCONF  "AUTC="   // Automation Complete = <Channel> (for confirmation)
#define RM_CONFIGP   "CPOL="   // Configure Polling Frequency = <byte>
#define RM_DATA      "DATA="   // Perform Data Operation = <Start / Stop>
#define RM_STATUS    "STAT="   // Request Status  = <Operation Type>


// Body Types:
#define M_BAD    "BADC"    // Bad Message
#define M_FALSE  "FALS"    // Logical False
#define M_TRUE   "TRUE"    // Logical True
#define M_ERROR  "ERRR"    // Unknown Error (not caught)
#define M_NOAUTH "NOAU"    // Not Authenticated (timeout)
#define M_START  "STRT"    // Start Operaton
#define M_END    "ENDD"    // End Operaton 
#define M_AUTOS  "AUTS"    // Automation Status
#define M_DATAS  "DATS"    // Data Status

