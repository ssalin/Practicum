/*
*
*  BT Serial Test
*
*
*/

#include "blu_defs.h"   // Macros & Prototypes
#include <Arduino.h>  // Arduino Type Definitions
#include <EEPROM.h>   // EEPROM Library
#include <Wire.h>     // I2C Library
#include <avr/wdt.h>  // Watchdog library

// Memory Usage Note:
// Global Values use a lot of memory!

auth_data auth_dat;     // Auth Data Struct - Note that using pointers caused weird problems...
bool device_authenticated = false;  // Device Host Auth Status
sensor_data sensor_dat; // Sensor Data Struct
auto_data auto_dat[NUM_RELAY];   // Automation Data
bool EEPROM_LD = false;           // EEPROM Load Flag
volatile bool Run = false;        // ISR Flag
volatile int wSetting;        //variable to store WDT setting, make it volatile since it will be used in ISR

// EEPROM Function Example:
// EEPROM.put(address, *foo);
// EEPROM.get(address, foo);
// Global Interrupt Functions Examples:
// cli();       // Disable all interrupts
// sei();       // Enable all interrupts

// Setup Loop (Run Once at Boot)
void setup() {
  
  auth_dat = {false,0,0};   // Init Auth Struct
//  sensor_dat = {0,false,0,0,0};
//  for (int i = 0; i < NUM_RELAY; i++){
//    auto_data_constructor(auto_dat[i]);
//  }

  // Get Polling Frequency
//  EEPROM.get(AUTH_ADDR, auth_dat);
//  if (auth_data.poll_freq == 0){ 
//      wSetting = 9;       // Set to default interrupt mode of 4s
//  }
//  else{
//      wSetting = auth_data.poll_freq    // Load in the EEPROM polling frequency
//  }

//  // Disable interrupts globally
//  cli(); 
//  // Reset Watchdog Timer
//  MCUSR &= ~(1<<WDRF);  
//  // Prepare watchdog for config change.
//  WDTCSR |= (1<<WDCE) | (1<<WDE);
//
//  // Prompt user for oscillator and choose timer mode and prescaler
//  getSettings();
//  if(wSetting == 1) WDTCSR = (1<<WDIE); //set for interrupt 16 ms
//  else if(wSetting == 2) WDTCSR = (1<<WDIE) | (1<<WDP0); //set for interrupt 32 ms
//  else if(wSetting == 3) WDTCSR = (1<<WDIE) | (1<<WDP1); //set for interrupt 64 ms
//  else if(wSetting == 4) WDTCSR = (1<<WDIE) | (1<<WDP1) | (1<<WDP0); //set for interrupt 0.125 s
//  else if(wSetting == 5) WDTCSR = (1<<WDIE) | (1<<WDP2); //set for interrupt 0.25 s
//  else if(wSetting == 6) WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP0); //set for interrupt 0.5 s
//  else if(wSetting == 7) WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1); //set for interrupt 1 s
//  else if(wSetting == 8) WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0); //set for interrupt 2 s
//  else if(wSetting == 9) WDTCSR = (1<<WDIE) | (1<<WDP3); //set for interrupt 4 s
//  else if(wSetting == 10) WDTCSR = (1<<WDIE) | (1<<WDP3) | (1<<WDP0); //set for interrupt 8 s
//    
//  //WDTCSR = (1<<WDE) | (1<<WDP3);  // 4sec, System Reset Mode
//  //WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);  // 2sec, Interrupt Mode
//  //WDTCSR = (1<<WDE) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);  // 2sec, System Reset Mode 
//
//  // Enable interrupts globally
//  sei();   
  Serial.begin(9600);       // Enable Serial
}

//This function gets the WDT setting from the user using Serial comm
//void getSettings(){
//  Serial.begin(BAUD_RATE);
//  Serial.println("Select the Oscillator setting: 1 --> 16 ms, 2 --> 32 ms, 3 --> 64 ms");
//  Serial.println("Select the Oscillator setting: 4 --> 0.125 s, 5 --> 0.25 s, 6 --> 0.5 ms");
//  Serial.println("Select the Oscillator setting: 7 --> 1 s, 8 --> 2 s, 9 --> 4 s, 10 --> 8 s");
//  //wait for user to input setting selection
//  while (!Serial.available()) { }
//  wSetting = Serial.parseInt(); //get entry and ensure it is valid
//  if(wSetting < 1 || wSetting > 10) {
//      wSetting = 9;
//   }
//   Serial.print("WDT setting is ");
//   Serial.println(wSetting);
//   Serial.println();
//   Serial.end(); //don't want to mix serial comm and interrupts together, serial communication uses interrupt and don't want to mix with watchdog
//}

// Interrupt vector, change flag status for main loop
//ISR(WDT_vect)
//{
//  Run = true;
//  Serial.println("INTERRUPT!!!");
//}

// constructor for automation data struct
void auto_data_constructor(auto_data foo){
  
  foo.en = 0;   // Enable
  foo.dec = 0;  // Descending set point
  foo.tog = 0;   // Toggle Relay
  foo.tmp = 0;   // Temperature Sensor
  foo.pres = 0;   // Pressure Sensor
  foo.hum = 0;   // Humidity sensor
  foo.ls = 0;   // Light sensor
  foo.pir = 0;   // PIR Motion Sensor

  foo.setpoint = 0.0;   // Setpoint 
  foo.t_duration = 0;   // Toggle Duration
}

// Main Loop:
void loop() {
  
  char mbuffer[MAX_MSG_SIZE + 1];  // Incoming Message Buffer
  int i = 0;  // Index for Clearning Buffer
  
  
  if(Serial.available() > 0){
    Serial.readBytesUntil('\n', mbuffer, MAX_MSG_SIZE);
    
    // Call Parser:
    char * pntr = &mbuffer[0];
    if(parseMessage(pntr, MAX_MSG_SIZE)){
     
     // Confirm Understood 
     
    }
    else{
    // Bad Message:
      Serial.println(M_BADMSG M_BAD);
    }
    

    // Clear Message Buffer
    for(i = 0; i < MAX_MSG_SIZE; i++){
      mbuffer[i]= '\0';
    }
  }

} // END LOOP //




// Incoming Message Parser
// Uses global message buffer (may be bad)
byte parseMessage(char * msg, int len){
 
  // array [i] = * (array + i)
  byte i = 0, j = 0, sel = NUM_RX_MSG + 1;    // For loop declaration
  char buf[MAX_MSG_SIZE + 1];       // static char with fixed size
 
  // Copy Message Type to Temp:
  for(i = 0; i < MESSAGE_LEN; i++){ // Cycle Array
      buf[i] = msg[i];
   }
   
   buf[MESSAGE_LEN] = '\0';
  
  // Debug:
  //Serial.println(buf);
  
  // Compare to Message Types:
  for(j = 0; j < NUM_RX_MSG; j++){  // For each message type

     if(!strcmp(buf, rx_msg[j])){
       
       sel = j; // Message Type
       
       // Clear Buffer:
       for(i = 0; i < MAX_MSG_SIZE + 1; i++){
         buf[i] = '\0';
       }
       
       for(i = 0; i < MAX_MSG_SIZE + 1; i++){
        buf[i] = msg[i + MESSAGE_LEN]; // BUF holds message body:
       }
       
       buf[i]= '\0';
       //continue;
       break;
     }
  }



  
    
  // Respond to message : rx_msg[j]
  switch (sel) {
      
      case 0:    // Automation Channel Select <VALUE>
        
        break;
        
      case 1:    // Automation Flag Set       <VALUE>
      
        break;
      
      case 2:    // Automation Set Point      <VALUE>
      
        break;
      
      case 3:    // Automation Duration       <VALUE>
      
        break;
      
      case 4:    // Automation Complete (conf) <VALUE>
      
        break;
      
      case 5:    // Configure Polling Freq.    <VALUE>
        return config_poll_freq(buf);
        break;
      
      case 6:    // Perform Data Operation (upload ready) <BODY>
        
        break;
      
      case 7:    // Perform Status <BODY>
        
        if(!strcmp(buf, M_AUTOS)){  // Report Automation Status
           
          return 1;  
        }
        
        if(!strcmp(buf, M_DATAS)){  // Report Data Status
         
          return 1; 
        }
        
        if(!strcmp(buf, M_AUTHS)){  // Report Authentication Status
         
         if(device_authenticated){
           Serial.println(M_AUTH M_TRUE);
         }
         else{
           Serial.println(M_AUTH M_FALSE);
         }
         
         return 1; 
        }
        
        Serial.println(M_HELLO M_TRUE);  // General Status Report
        return 1;
      
      break;
      
      case 8:    // Perform Auth <KEY> 
          //Serial.println(buf);
          return authenticate(atoi(buf));  
          break;
       
      default:
          // Message parsed but not understood. 
          return 0;
          break;
  }
  
  return 0;
}

//// Perform Data Operation
//byte data_operation(char value){
//  if (strcmp(value,M_TRUE){
//    Serial.println(RM_DATA M_START);
//    return 1;
//  }
//  else{
//    if (strcmp(value,M_FALSE){
//        Serial.println(RM_DATA M_END);
//        return 1;
//    }
//    else{
//      return 0;
//    }
//  }
//}
//
//
//// Set Polling Frequency 
//byte config_poll_freq(byte value){
//  if (value > 1 and value <11){
//      auth_dat.poll_freq = value;
//      // Serial.println(RM_CONFIGP M_TRUE);
//      return 1;
//  }
//  else{
//      // Serial.println(RM_CONFIGP M_FALS);
//      return 0;
//  }
//}
  
// Authenticate Device:
// Checks auth_dat global structure
// Updates global "authenticated" bool
byte authenticate(int key){

  // No Key Set: Update
  if (auth_dat.auth_set == false){
    auth_dat.key = key;
    auth_dat.auth_set = true;
    device_authenticated = true;
    // Store AUTH STRUCT TO EEPROM //
    Serial.println(M_AUTH M_TRUE);  // Auth + True
    return 1;
  }
  
  // Key Set - Validate:
  else{
     
   if(auth_dat.key == key){
      device_authenticated = true;
      Serial.println(M_AUTH M_TRUE);  // Auth + True 
      return 1;
    }
    else{
      Serial.println(M_AUTH M_FALSE); // Auth + False
      return 1;
    }
  
  }
  return 0;
}
