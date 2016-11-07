/*
 * 
 * BluDaq Firmware V0.1
 * October 2016
 * 
 */

#include "blu_defs.h"   // Macros & Prototypes


// Init:
void setup() {

   #ifdef SERIAL_EN
    Serial.begin(9600);
  #endif

  #ifdef DEBUG_EN
    Serial.print("DEBUG ENABLED\n");
  #endif

}


// Main:
void loop() {


}


// Load from EEPROM
void load(int address)
{
  if (address < EEPROM_SIZE)
  { 
    Serial.println(EEPROM.read(address));
  }
  else
  {
    Serial.println("Address is out of range");  
  }
}
// Store to EEPROM
void store(int address, int content)
{
  EEPROM.write(address,content);  
  if (address < EEPROM_SIZE && content != NULL)
  { 
    EEPROM.write(address,content);
  }
  else
  {
    Serial.println("Address is out of range"); 
}

