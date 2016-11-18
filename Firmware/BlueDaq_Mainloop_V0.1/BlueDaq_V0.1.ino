/*
 * 
 * BluDaq Firmware V0.2
 * Nov 2016
 * 
 */
#include <TimerOne.h>
#include "blu_defs.h"   // Macros & Prototypes
#include <Arduino.h>  // Arduino Type Definitions
#include <EEPROM.h>   // EEPROM Library
#include <Wire.h>     // I2C Library


// Global Vars:
bool EEPROM_LD = false;           // EEPROM Load Flag
volatile bool Run = false;        // ISR Flag
auth_data *auth_dat;              // Auth Data
sensor_data *sensors_dat;         // Sensor Data
auto_data *auto_dat[NUM_RELAY];   // Automation Data
int default_delay = 327680000;    // 32.758kHz 
int f_delay;

// EEPROM Function Example:
// EEPROM.put(address, *foo);
// EEPROM.get(address, foo);
// noInterrupts();                //disable interrupt
// interrupts();                  //re-enable interrupt

// Setup *Run-Once
void setup(){
    
  // Constructor for all global structs
  *auth_dat = {false,0,0.0,0.0};
  *sensors_dat = {0,false,0.0,0.0,0.0};
  for (int i = 0; i < NUM_RELAY; i++){
    
    auto_data_constructor(auto_dat[i]);
    Serial.print(F(" ======================== "));
    Serial.print(sizeof(*auto_dat[i]));
  }

  // get timer value from auth.delay (polling frequency)
  if (auto_dat->poll_freq == null){ 
      f_delay = default_delay;
  }
  else{
      f_delay = auto_dat->poll_freq * 1000    // Result need to be in microseconds
  }
  
  Timer1.initialize(f_delay);     // Timer's period in microseconds. Library default is 1 us or 1 Mhz 
  Timer1.attachInterrupt(INTR);   // Interrupts and execute INTR function
  Serial.begin(9600);             // Enable Serial

}

// constructor
void auto_data_constructor(auto_data *foo){
  
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

// The interrupt will check the ISR status, 
void INTR(void){
  Run = true
}

// Main Loop
void loop() {
  /* Empty loop */
  if (Run){
      // noInterrupts();                //disable interrupt

      // Test EEPROM Flag
      if (EEPROM_LD == false){
          // Load EEPROM
          EEPROM_LD = true;  
          // call Store to Automation Struct
      }
      
      // If (automation){
            // Read Sensor Data and store sensor to struct
      // }
      
      // If (Serial){
              // if (Host is not Authenticated){
                  // Return not authorized
              // }

              // else{
                    // if (automation){
                        // call store to EEPROM function
                        // Update automation struct function
                    // }
            
            
                 // upload sensor data function
              // }
            
      //  serial flush()
      // }
      
      
      // Check for automation, do automation if found
      }
      Run = false;
      // interrupts();                  //re-enable interrupt
  }
}

// Store to EEPROM
void store_EEPROM(){


}


// Load sensor data
void load_data(){
  
  
}



