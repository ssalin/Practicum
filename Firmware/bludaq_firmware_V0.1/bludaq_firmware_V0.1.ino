/*
 * 
 * BluDaq Firmware V0.1
 * Nov 2016
 * 
 */

#include "blu_defs.h"   // Macros & Prototypes

// Global Vars:
bool EEPROM_LD = false;           // EEPROM Load Flag
auth_data *auth_dat;              // Auth Data
sensor_data *sensors_dat;         // Sensor Data
auto_data *auto_dat[NUM_RELAY];   // Automation Data

// EEPROM Function Example:
// EEPROM.put(address, *foo);
// EEPROM.get(address, foo);

// Setup *Run-Once
void setup() {
  
  Serial.begin(9600);       // Enable Serial

}

// Main Loop
void loop() {
  /* Empty loop */
  
  
}





