/*
 * BluDaq Firmware
 * Macro Definitions File
 * Nov 2016
 * 
 */
 
 // Required for Type Definitions:
 #include <Arduino.h>



//
// Debug and Option Flags:
//

//#define DEBUG_EN   // Debug print statements
#define SERIAL_EN  // Enable Serial Debug Statements



//
// Other Definitions:
//

#define PRECISION 2           // Float to String Precision 
#define NUM_RELAY 2           // Number of Relays
//#define BAUD_RATE 115200    // Better Baud Rate for Serial
#define BAUD_RATE 9600        // Typical Baud Rate for Serial
#define MAX_MSG_SIZE 24       // 24 Chars + Terminator
#define RELAY_PW_DELAY  20


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
  bool disp_data;   // Sensor Upload Toggle
  int key;          // Auth-key
  byte poll_freq;   // Polling Frequency (power management)
} auth_data;

// Sensor Data - Stores "current" values
typedef struct{
  bool PIR;             // Motion Sensor State
  int ls;               // Light Sensor Analog Value (0-1023)
  float temp;           // Temperature
  float pressure;       // Pressure
  float humidity;       // Humidity
} sensor_data;


// Sensor Values:
#define NUM_SENSORS 5
enum sensor_sel {TEMP, PRES, HUMI, LIGHT, PIR};

typedef struct {

  // Relay 0 Automation Flags (1 byte):
  bool en;             // Enable
  bool desc;            // Descending set point
  bool toggle;         // Toggle Relay (for duration
  enum sensor_sel device;   // Device Selection
  float setpoint;      // Setpoint 
  int t_duration;      // Toggle Duration
} auto_data;
 


//
// EEPROM Settings:
//

#define EEPROM_SIZE 1024;     // Size of EEPROM in Bytes
#define AUTH_ADDR 0           // Address of Authentication Structure
#define AUTO_ADDR 12          // Address of Automation Structure(s)
#define AUTO_SIZE 7           // Size of Automation Struct, 7 per struct, 14 for both structs
#define AUTH_SIZE 11           // Size of Authentication Struct


//
// Serial Communication - Message Stack
// Message Structure:  <Send Type> = <Body Type> or <Value>
// Transmit Messages: From Device to Serial Host
// Recieve Messages: From Serial Host to Device
// Body Messages: Standard message types (not values) used by host and device
//

// Message Type Sizes:
#define MESSAGE_LEN 5    // Size of all Message Types
#define BODYMSG_LEN 11   // Size of all Messages using a <Body> format (not data)
#define BODYSIZE 4       // Size of all Bodies
#define NUM_RX_MSG 10     // Number of RX Message Types
#define NUM_TX_MSG 12    // Number of TX Message Types
#define NUM_BODY_MSG 10  // Number of body message types

// Transmit Message Types: 
#define M_BADMSG "UNKO="   // Bad Message = <Incoming Message>
#define M_HELLO  "HELO="   // Hello = <UUID?>
#define M_AUTH   "AUTH="   // Authentication Status = <T/F>
#define M_SLEEP  "SLEP="   // Device will sleep = <Duration>
#define M_ERROR  "EROR="   // Device Error = <Error Message or ID>
#define M_TEMP   "DTMP="    // Temperature = <Value>
#define M_HUMID  "DHUM="    // Humidity = <Value>
#define M_PRESS  "DPRS="    // Pressure = <Value>
#define M_PHOTO  "DRLS="    // Photoresistor = <Value>
#define M_PIR    "DPIR="    // Motion Sensor = <T/F>
#define M_RL0    "DRL0="    // Relay 0 State = <T/F> (on / off)
#define M_RL1    "DRL1="    // Relay 1 State = <T/F> (on / off)

// Array of TX Messages
const char * tx_msg[] {M_BADMSG, M_HELLO, M_AUTH, M_SLEEP, M_ERROR, M_TEMP, M_HUMID, M_PRESS, M_PHOTO, M_PIR, M_RL0, M_RL1};

// Recieve Message Types: <Recieve Type> = <Body Type> or <Value>
#define RM_AUTOCHAN  "AUTN="   // Sensor Select = <Channel Value> "01"
#define RM_AUTOFLAG  "AUTF="   // Automation Flag Set = <Byte>
#define RM_AUTOSET   "AUTS="   // Automation Setpoint = <Float> (4-byte)
#define RM_AUTODUR   "AUTD="   // Automation Duration = <Int> (2-byte)
#define RM_AUTOCONF  "AUTC="   // Automation Complete = <Channel> (for confirmation)
#define RM_CONFIGP   "CPOL="   // Configure Polling Frequency = <byte>
#define RM_DATA      "DATA="   // Perform Data Operation = <Start / Stop>
#define RM_STATUS    "STAT="   // Request Status  = <Operation Type>
#define RM_AUTHKEY   "AUTK="   // Request Auth = <KEY>
#define RM_AUTOTOG   "AUTG="    // Enable / Disalbe Toggle

// Array of RX Messages
const char * rx_msg[] {RM_AUTOCHAN, RM_AUTOFLAG, RM_AUTOSET, RM_AUTODUR, RM_AUTOCONF, RM_CONFIGP, RM_DATA, RM_STATUS, RM_AUTHKEY, RM_AUTOTOG};

// Body Types:
#define M_BAD    "BADC"    // Bad Message
#define M_FALSE  "FALS"    // Logical False
#define M_TRUE   "TRUE"    // Logical True
#define M_ERR    "ERRR"    // Unknown Error (not caught)
#define M_NOAUTH "NOAU"    // Not Authenticated (timeout)
#define M_START  "STRT"    // Start Operaton
#define M_END    "ENDD"    // End Operaton 
#define M_AUTOS  "AUTS"    // Automation Status
#define M_DATAS  "DATS"    // Data Status
#define M_AUTHS  "ATHS"     // Authentication Status
#define M_BME280  "BMES"    // BME280 Status

// Array of Body Messages:
const char * body_msg[] = {M_BAD, M_FALSE, M_TRUE, M_ERROR, M_NOAUTH, M_START, M_END, M_AUTOS, M_DATAS, M_AUTHS, M_BME280 };
