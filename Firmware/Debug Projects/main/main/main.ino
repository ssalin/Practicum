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
sensor_data sensor_dat; // Sensor Data Struct
auto_data auto_dat[NUM_RELAY];   // Automation Data
auto_data temp;         // Hold new automation data
bool device_authenticated = false;  // Device Host Auth Status
bool data_ready = false;  // Device Host Auth Status
bool automation_ready = false;  // Device Host Auth Status
volatile bool EEPROM_LD = false;           // EEPROM Load Flag
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
  
  auth_dat = {false,0,0};                 // Init Auth Struct
  sensor_dat = {0,false,0,0,0};           // Init sensor data struct
  for (int i = 0; i < NUM_RELAY; i++){    //Init automation data struct
      auto_dat[i].en = 0;                 // Enable
      auto_dat[i].dec = 0;                // Descending set point
      auto_dat[i].tog = 0;                // Toggle Relay
      auto_dat[i].tmp = 0;                // Temperature Sensor
      auto_dat[i].pres = 0;               // Pressure Sensor
      auto_dat[i].hum = 0;                // Humidity sensor
      auto_dat[i].ls = 0;                 // Light sensor
      auto_dat[i].pir = 0;                // PIR Motion Sensor
      auto_dat[i].setpoint = 0.0;         // Setpoint 
      auto_dat[i].t_duration = 0;         // Toggle Duration
  }

  // Get Polling Frequency
//  EEPROM.get(AUTH_ADDR, auth_dat);
//  if (auth_dat.poll_freq == 0){ 
//      wSetting = 9;                     // Set to default interrupt mode of 4s
//  }
//  else{
//      wSetting = auth_dat.poll_freq;    // Load in the EEPROM polling frequency
//      Serial.println(wSetting);
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

// Main Loop:
//void loop() {
//  /* Empty loop */
//  if (Run){
//      // cli();                       //disable interrupt
//      // Test EEPROM Flag
//      if (EEPROM_LD == false){
//          EEPROM_LD = true;
//          // Load EEPROM data to auth and auto structs         
//          // EEPROM.get(0, auth_dat);
//          // EEPROM.get(12, auto_dat[1]);
//          // EEPROM.get(19, auto_dat[0]);  
//      }
//
//      if (automation is true){        // pseudo if statement, unsure of what to compare
//        // how to read in these values? through the parser?
//        read_sensor(7,True,73.2,50.6,20.4);
//      }
//      
//       If (Serial){
//               if (device_authenticated == false){    //Host is not Authenticated
//                  Serial.println(M_AUTHS M_FALSE)     // Return not authorized
//               }
//
//               else{
//                     if (new automation){       // How to detect new automation? through parser? show how
//                        auto_data auto new      // holds new automation data
//                        if (automation for relay 1?){
//                           EEPROM.put(12, auto_new);
//                           auto_dat[1] = auto_new;
//                        }    
//                        else{     
//                           EEPROM.put(19, auto_new);
//                           auto_dat[0] = auto_new;
//                        }
//                     }
//            
//                 // upload sensor data to BT device
//                 incoming_message();            // Call BT function to send serial data to app
//               }
//            
//         Serial.flush()
//       }
//      
//      
//      // If (automation), do automation if found
//      }
//      Run = false;
//      // sei();                  //re-enable interrupt
//}

// Receiving message
//void incoming message(){
void loop(){  
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
  Serial.println(msg);
  Serial.println(buf);
  
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


  byte relay;
  // Respond to message : rx_msg[j]
  switch (sel) {
      
      case 0:    // Automation Channel Select <VALUE>
//          relay = select_channel(buf);
         return 1;
        break;
        
      case 1:    // Automation Flag Set       <VALUE>
//         if (relay == 1){
//             return auto_flag_set(buf,auto_dat[1]);
//         }
//         else{
//             return auto_flag_set(buf,auto_dat[0]);
//         }
        break;
      
      case 2:    // Automation Set Point      <VALUE>
//         if (relay == 1){
//             return auto_set_point(buf,auto_dat[1]);
//         }
//         else{
//             return auto_set_point(buf,auto_dat[0]);
//         }
        break;
      
      case 3:    // Automation Duration       <VALUE>
//         if (relay == 1){
//             return auto_duration(buf,auto_dat[1]);
//         }
//         else{
//             return auto_duration(buf,auto_dat[0]);
//         }
        break;
      
      case 4:    // Automation Complete (conf) <VALUE>
//         return automation_done(buf,relay);                     // Not really sure what to do with this option
        break;
      
      case 5:    // Configure Polling Freq.    <VALUE>
//        return config_poll_freq(buf);
        break;
      
      case 6:    // Perform Data Operation (upload ready) <BODY>
        if(strcmp(buf,M_TRUE)){
            Serial.println(M_PHOTO sensor_dat.ls);
            Serial.println(M_PIR sensor_dat.PIR);
            Serial.println(M_TEMP sensor_dat.temp);
            Serial.println(M_PRESS sensor_dat.pressure);
            Serial.println(M_HUMID sensor_dat.humidity);
            Serial.println(RM_DATA M_TRUE);     
        }
        else{
            Serial.println(RM_DATA M_FALS);
        }
        return 1;
        break;
      
      case 7:    // Perform Status <BODY>
        Serial.println( "I am in here ");
        Serial.println(buf);
        char tmp[3];
        for(byte t = 0; t < MESSAGE_LEN ; t++){
          tmp[t] = buf[t];   // tmp holds message body:
        }
        Serial.println(tmp);
        
        if(strcmp(tmp, M_AUTOS)){  // Report Automation Status
           if(automation_ready){
             Serial.println("AUTO" M_TRUE);
           }
           else{
             Serial.println("AUTO" M_FALSE);
           }
           
          return 1;  
        }
        
        if(strcmp(tmp, M_DATAS)){  // Report Data Status
           if(data_ready){
             Serial.println("DATA" M_TRUE);
           }
           else{
             Serial.println("DATA" M_FALSE);
           }
           
          return 1; 
        }
        
        if(strcmp(tmp, M_AUTHS)){  // Report Authentication Status      
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
          Serial.println(M_ERROR);
          return 0;
          break;
  }
  
  return 0;
}


// Sensor data upload
void read_sensor(int ls, bool PIR, float temp, float pressure, float humidity){
  sensor_dat.ls = ls;
  sensor.dat.PIR = PIR;
  sensor.dat.temp = temp;
  sensor.dat.pressure = pressure;
  sensor.dat.humidity = humidity;
  Serial.println(RM_DATA M_TRUE);
}


// set automation flag
byte auto_flag_set(byte value ,byte rel){
  // Select relay
  auto_dat[rel].en = bitRead(value,0);
  auto_dat[rel].dec = bitRead(value,1);
  auto_dat[rel].tog = bitRead(value,2);
  auto_dat[rel].tmp = bitRead(value,3);
  auto_dat[rel].pres = bitRead(value,4);
  auto_dat[rel].hum = bitRead(value,5);
  auto_dat[rel].ls = bitRead(value,6);
  auto_dat[rel].pir = bitRead(value,7);
  Serial.println(RM_AUTOFLAG M_TRUE);
  return 1;
}

// set automation set point
byte auto_set_point(int value ,byte rel){
  // Select relay
  auto_dat[rel].setpoint = value;
  Serial.println(RM_AUTOSET M_TRUE);
  return 1;
}


// set automation duration
byte auto_duration(int value ,byte rel){
  // Select relay
  auto_dat[rel].t_duration = value;
  Serial.println(RM_AUTODUR M_TRUE);
  return 1;
}


// Automation struct Channel selection
byte select_channel(byte channel){
    if (channel == "01"){
        Serial.println(M_RL1 M_TRUE);
        return 1;
    }
    else{
        Serial.println(M_RL0 M_TRUE);
        return 0;
    }
}


// Set Polling Frequency 
byte config_poll_freq(byte value){
  if (byte(value) > 1 and byte(value) <11){         
      auth_dat.poll_freq = value;
      return 1;
  }
  else{
      return 0;
  }
}


// Perform Data Operation
byte data_operation(char value){
  if (strcmp(value,M_TRUE){
    Serial.println(M_DATAS M_START);
    return 1;
  }
  else{
    if (strcmp(value,M_FALSE){
        Serial.println(M_DATAS M_END);
        return 1;
    }
    else{
      Serial.println(M_DATAS ERRR);
      return 0;
    }
  }
}


  
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
