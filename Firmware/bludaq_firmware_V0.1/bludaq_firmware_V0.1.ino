/*
 * 
 * BluDaq Firmware V0.1
 * October 2016
 * 
 */

#include "blu_defs.h"   // Macros & Prototypes
#include "Wire.h"

// Init:
void setup() {

    Wire.begin();   
   #ifdef SERIAL_EN
    Serial.begin(9600);
  #endif

  #ifdef DEBUG_EN
    Serial.print("DEBUG ENABLED\n");
  #endif

}


// Main:
void loop() {
  
    int addr = 0;
    byte value = 0;

    // Check bluetooth flag is connected

    // Write to EEPROM memory
    writeaddr(addr,value);

    // Read from EEPROM memory
    byte readval = readaddr(addr);

}

// Write to sensor value to address
void writeaddr(int address, byte val){
  
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB
     
  Wire.write(val);
  Wire.endTransmission();
}

// Read from memory address
byte readaddr(int address){
  
  byte rData = 0xFF;
  
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  
  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB
  Wire.endTransmission();  

  Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);  

  rData =  Wire.read();
  return rData;
}






// Load from EEPROM
void load(int address){
  
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
void store(int address, int content){
  
  EEPROM.write(address,content);  
  if (address < EEPROM_SIZE && content != NULL)
  { 
    EEPROM.write(address,content);
  }
  else
  {
    Serial.println("Address is out of range"); 
  }
}
