/*
 * 
 * BluDaq Firmware V0.1
 * October 2016
 * 
 */

#include "blu_defs.h"   // Macros & Prototypes
#include <EEPROM.h>

// Init: initialize serial begins and create struct pointers which is defined inside blu_defs.h
void setup() {

  Serial.begin(9600);

  auto_data temp;
  int eeAddress = 0;   //Location we want the data to be put.

  //simple function calls to get or put new struct object to memory address.
  //put(eeAddress, &temp);
  //delay(3000);
  get(eeAddress);
}


// This funciton will create new content for the struct object and then puts it in the specified memory location
void put(int address, auto_data *foo)
{
  foo->en_R0 = 1;   // Enable
  foo->dec_0 = 0;   // Descending set point
  foo->tog_0 = 1;   // Toggle Relay
  foo->temp_0 = 0;  // Temperature Sensor
  foo->pres_0 = 1;  // Pressure Sensor
  foo->humi_0 = 1;  // Humidity sensor
  foo->ls_0 = 0;    // Light sensor
  foo->pir_0 = 0;   // PIR Motion Sensor
    
  foo->setpoint_0 = 50.0;   // Setpoint 
  foo->t_duration_0 = 300;   // Toggle Duration

  EEPROM.put(address, *foo);
}

// This funciton will create a new object and get the information from the specified memory address 
void get(int address)
{
  auto_data foo;
  EEPROM.get(address, foo);
  Serial.print(F("==========================\n"));
  Serial.println( foo.en_R0 ); 
  Serial.println( foo.dec_0 );      
  Serial.println( foo.tog_0 );
  Serial.println( foo.temp_0 );
  Serial.println( foo.pres_0 );
  Serial.println( foo.humi_0 );
  Serial.println( foo.ls_0 );
  Serial.println( foo.pir_0 );
  Serial.println( foo.setpoint_0 );
  Serial.println( foo.t_duration_0 );
  
}

void loop() {
  /* Empty loop */
}





