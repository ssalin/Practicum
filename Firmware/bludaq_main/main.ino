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
int default_delay = 327680000;    // deafult delay in ms 
int f_delay;                      // variable used to pass delay

// EEPROM Function Example:
// EEPROM.put(address, *foo);
// EEPROM.get(address, foo);
// noInterrupts();                //disable interrupt
// interrupts();                  //re-enable interrupt


// Setup *Run-Once
void setup(){
    
  // Constructor for all global structs, only do this once. 
  *auth_dat = {false,0,0.0,0.0};
  *sensors_dat = {0,false,0.0,0.0,0.0};
  for (int i = 0; i < NUM_RELAY; i++){
    auto_data_constructor(auto_dat[i]);
    if (DEBUG_EN){                // Print when in debug mode
      Serial.print(F(" ======================== "));
      Serial.print(sizeof(*auto_dat[i]));
    }
  }

  // get timer value from auth.delay (polling frequency)
  EEPROM.get(AUTH_ADDR,*auth_dat);
  if (auth_data.poll_freq == 0.0){ 
      f_delay = default_delay;
  }
  else{
      f_delay = auth_data.poll_freq * 1000    // Result need to be in microseconds
  }
  
  Timer1.initialize(f_delay);     // Timer's period in microseconds. Library default is 1 us or 1 Mhz 
  Timer1.attachInterrupt(INTR);   // Interrupts and execute INTR function
  Serial.begin(9600);             // Enable Serial

}

// constructor for automation data struct
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
          // auth_data authen;              // Auth Data
          // auto_data relay1,relay2;         // Sensor Data
          // EEPROM.get(0, authen);
          // EEPROM.get(12, relay1);
          // EEPROM.get(19, relay2);
          EEPROM_LD = true;  
          // call Store to Automation Struct
      }
      
      // If (automation){
            // read_store_sensordata()  //Read Sensor Data and store sensor to struct
    
      // }
      
      // If (Serial){
              // if (Host is not Authenticated){
                  // Return not authorized
              // }

              // else{
                    // if (new automation){
                        // call store to EEPROM 
                        // Update automation struct 
                    // }
            
            
                 // upload sensor data to BT device
              // }
            
         Serial.flush()
      // }
      
      
      // Check for automation, do automation if found
      }
      Run = false;
      // interrupts();                  //re-enable interrupt
  }
}

// Read and store sensor data struct
void read_store_sensordata(void){
    // Read from sensors and store in sensors_dat struct
    *sensors_dat->ls = 8;
    *sensors_dat->PIR = true;
    *sensors_dat->temp = 79.9;
    *sensors_dat->pressure = 23.9;
    *sensors_dat->humidity = 43.0;
}

// Reset sensor data struct
void reset_sensordata(void){
    *sensors_dat->ls = 0;
    *sensors_dat->PIR = false;
    *sensors_dat->temp = 0.0;
    *sensors_dat->pressure = 0.0;
    *sensors_dat->humidity = 0.0;
  
}


// Function to update EEPROM automation struct
void store_EEPROM(){


}


// Load sensor data
void upload_sensordata(){
  
  
}
