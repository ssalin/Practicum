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
int rel;


// EEPROM Function Example:
// EEPROM.put(address, *foo);
// EEPROM.get(address, foo);
// noInterrupts();                //disable interrupt
// interrupts();                  //re-enable interrupt


// Setup *Run-Once
void setup(){
  Serial.begin(9600);             // Enable Serial
  inter(0,32000000);

}

void print_func(int i){
  Serial.println("This is the relay value " + String(i));
}

// The interrupt will check the ISR status, 
void INTR(void){
  Serial.println("Turned off relay");
  print_func(rel);
  Timer1.detachInterrupt();
}

void inter(int relay, int duration){
  rel = relay;
  int default_delay = 327680000;    // deafult delay in ms 
  int f_delay;                      // variable used to pass delay
  if (duration == 0){ 
      f_delay = default_delay;
  }
  else{
      f_delay = duration * 100000000;    // Result need to be in microseconds
  }
  
  Timer1.initialize(duration);     // Timer's period in microseconds. Library default is 1 us or 1 Mhz 
  Timer1.attachInterrupt(1,INTR);   // Interrupts and execute INTR function
  Serial.println("interrupt is now on");
}



// Main Loop
void loop() {

}












//// constructor for automation data struct
//void auto_data_constructor(auto_data *foo){
//  
//  foo->en = 0;   // Enable
//  foo->dec = 0;  // Descending set point
//  foo->tog = 0;   // Toggle Relay
//  foo->tmp = 0;   // Temperature Sensor
//  foo->pres = 0;   // Pressure Sensor
//  foo->hum = 0;   // Humidity sensor
//  foo->ls = 0;   // Light sensor
//  foo->pir = 0;   // PIR Motion Sensor
//
//  foo->setpoint_0 = 0.0;   // Setpoint 
//  foo->t_duration_0 = 0;   // Toggle Duration
//}







//
//// Read and store sensor data struct
//void read_store_sensordata(void){
//    // Read from sensors and store in sensors_dat struct
//    *sensors_dat->ls = 8;
//    *sensors_dat->PIR = true;
//    *sensors_dat->temp = 79.9;
//    *sensors_dat->pressure = 23.9;
//    *sensors_dat->humidity = 43.0;
//}
//
//// Reset sensor data struct
//void reset_sensordata(void){
//    *sensors_dat->ls = 0;
//    *sensors_dat->PIR = false;
//    *sensors_dat->temp = 0.0;
//    *sensors_dat->pressure = 0.0;
//    *sensors_dat->humidity = 0.0;
//  
//}
//
//
//// Function to update EEPROM automation struct
//void store_EEPROM(){
//
//
//}
//
//
//// Load sensor data
//void upload_sensordata(){
//  
//  
//}
