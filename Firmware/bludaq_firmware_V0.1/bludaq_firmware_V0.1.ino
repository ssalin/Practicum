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
  
  Serial.print(F(" ======================== "));
  Serial.print(auth_dat->touch);
  Serial.print(F(" "));
  Serial.print(sizeof(*sensors_dat));

  auto_data automa;
  auth_data auth
  // Store information to EEPROM
  put(AUTH_ADDR);
  put(AUTO_ADDR);
  put(AUTO_ADDR + AUTO_ADDR);
}

// This funciton will create new content for the struct object and then puts it in the specified memory location
void put(int address, auto_data *foo)
{
  if (sizeof(*foo) == 11){
      foo->auth_set = false;
      foo->touch = 0;
      foo->key = 0.0;
      foo->poll_freq = 0.0;
  }
  else{
      foo->en_R0 = 1;   // Enable
      foo->dec_0 = 1;   // Descending set point
      foo->tog_0 = 1;   // Toggle Relay
      foo->temp_0 = 1;  // Temperature Sensor
      foo->pres_0 = 1;  // Pressure Sensor
      foo->humi_0 = 1;  // Humidity sensor
      foo->ls_0 = 1;    // Light sensor
      foo->pir_0 = 1;   // PIR Motion Sensor
    
      foo->setpoint_0 = 70.0;   // Setpoint 
      foo->t_duration_0 = 600;   // Toggle Duration
  }
  EEPROM.put(address, *foo);
}


void get(int address)
{
  auto_data foo;
  EEPROM.get(address, foo);
  if (sizeof(foo) == 11){
      Serial.print(F("==========================\n"));
      Serial.println( foo.auth_set );
      Serial.println( foo.touch );
      Serial.println( foo.key );
      Serial.println( foo.poll_freq );
  }
  else{
      Serial.print(F("==========================\n"));
      Serial.println( foo.en ); 
      Serial.println( foo.dec );      
      Serial.println( foo.tog );
      Serial.println( foo.tmp );
      Serial.println( foo.pres );
      Serial.println( foo.hum );
      Serial.println( foo.ls );
      Serial.println( foo.pir );
      Serial.println( foo.setpoint );
      Serial.println( foo.t_duration );
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

  foo->setpoint = 0.0;   // Setpoint 
  foo->t_duration = 0;   // Toggle Duration
}




// Main Loop
void loop() {
  /* Empty loop */
  
  
}





