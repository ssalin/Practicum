

/*
 * 
 * BluDaq Firmware V0.1
 * Nov 2016
 * 
 */

#include "blu_defs.h"   // Macros & Prototypes
#include <Arduino.h>  // Arduino Type Definitions
#include <EEPROM.h>   // EEPROM Library
#include <Wire.h>     // I2C Library

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
  
  // Constructor for all global structs
  *auth_dat = {false,0,0.0,0.0};
  *sensors_dat = {0,false,0.0,0.0,0.0};
  for (int i = 0; i < NUM_RELAY; i++)
  {
    auto_data_constructor(auto_dat[i]);
    Serial.print(F(" ======================== "));
    Serial.print(sizeof(*auto_dat[i]));
  }

}

// constructor
void auto_data_constructor(auto_data *foo)
{
  foo->en = 0;   // Enable
  foo->dec = 0;  // Descending set point
  foo->tog = 0;   // Toggle Relay
  foo->tmp = 0;   // Temperature Sensor
  foo->pres = 0;   // Pressure Sensor
  foo->hum = 0;   // Humidity sensor
  foo->ls = 0;   // Light sensor
  foo->pir = 0;   // PIR Motion Sensor

  foo->setpoint_0 = 0.0;   // Setpoint 
  foo->t_duration_0 = 0;   // Toggle Duration
}


// Main Loop
void loop() {
  /* Empty loop */
  
  
}





