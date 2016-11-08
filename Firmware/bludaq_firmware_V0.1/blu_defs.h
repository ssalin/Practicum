/*
 * 
 * 
 * BluDaq Firmware
 * Macro Definitions File
 * 
 */

//
// Structs  //
//


// Sensor Data
typedef struct{

  int ls;               // Light Sensor Analog Value (0-1023)
  bool PIR;             // Motion Sensor State
  float temp;           // Temperature
  float pressure        // Pressure
  float humidity        // Humidity
  float sealevel_alt;   // Provided by Host?
  
} sensor_data, *s_data;

// Automation 
//
// Note: Sizes
// bool = 1 byte 
// int / word = 2 byte
// long / float/ double = 4 byte
//

typedef struct {

  // Relay 0 Automation Flags (1 byte):
  byte en_R0 : 1;   // Enable
  byte dec_0 : 1;   // Descending set point
  byte tog_0 : 1;   // Toggle Relay
  byte temp_0 : 1;  // Temperature Sensor
  byte pres_0 : 1;  // Pressure Sensor
  byte humi_0 : 1;  // Humidity sensor
  byte ls_0 : 1;    // Light sensor
  byte pir_0 : 1;   // PIR Motion Sensor

  float setpoint_0;   // Setpoint 
  int t_duration_0;   // Toggle Duration

  // Relay 1 Automation Flags:
  byte en_R1 : 1;   // Enable
  byte dec_1 : 1;   // Descending set point
  byte tog_1 : 1;   // Toggle Relay
  byte temp_1 : 1;  // Temperature Sensor
  byte pres_1 : 1;  // Pressure Sensor
  byte humi_1 : 1;  // Humidity sensor
  byte ls_1 : 1;    // Light sensor
  byte pir_1 : 1;   // PIR Motion Sensor
  
  float setpoint_1;   // Setpoint 
  int t_duration_1;   // Toggle Duration
  
} auto_data, *a_data;
 
//
// Debug //
//

//#define DEBUG_EN   // Comment to disable serial debugging 
#define SERIAL_EN  // Comment to disable serial com


//
// EEPROM //
//

#define EEPROM_SIZE=1024;
#define EEPROM_FLAG=False;

//
// Pins //
//

#define BT_DSR          // BT DSR Pin
#define BT_STS 3        // BT Status Pin 
#define PIR0 4          // PIR motion sensor (active low)
#define R0_SET 5        // Set Relay 0
#define R0_RSET 6       // Reset Relay 0
#define R1_SET 7        // Set Relay 1
#define R1_RSET 8       // Reset Relay 1
#define LIGHT_SENSOR 0  // Analog 5v input

