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
