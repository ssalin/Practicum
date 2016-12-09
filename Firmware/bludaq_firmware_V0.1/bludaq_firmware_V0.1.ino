
/*
 * 
 * BluDaq Firmware V0.1
 * Nov 2016
 * 
 */

#include "Timer.h"
#include "blu_defs.h"         // Macros & Prototypes
#include <Arduino.h>          // Arduino Type Definitions
#include <EEPROM.h>           // EEPROM Library
#include <Wire.h>             // I2C Library
#include <SPI.h>              // SPI Library (needed for class)
#include <Adafruit_Sensor.h>  // BME280 
#include <Adafruit_BME280.h>  // BME280


// Globals:

Adafruit_BME280 bme;           // I2C BME280 Instance
auth_data auth_dat;            // Authentication Token
bool session_auth = false;     // Session Authenticated
bool Run = false;
int relay_tog;

// Setup Loop:
void setup() {

  // Pin Config:
    pinMode(R0_SET, OUTPUT);
    pinMode(R0_RSET, OUTPUT);
    pinMode(R1_SET, OUTPUT);
    pinMode(R1_RSET, OUTPUT);
    pinMode(PIR0, INPUT);
  
  // Init Serial
  Serial.begin(BAUD_RATE);
  
  // Init BME280
  if (!bme.begin()) {
    // Report Error with I2C Connection to BME280
    Serial.println(M_ERROR M_BME280);

  }
  
  // Config Auth Token
  auth_dat = {0, 0, 0, 0, 0, 0};
}

// Main Loop:
void loop() {

  // Local Variables:
  sensor_data s_dat = {false, 0, 0, 0, 0};   // Sensor Data Struct
  auto_data auto_dat[NUM_RELAY];             // Automation Structures 
  byte ee_test;
  byte unique = UNIQUE;
  
  // Initialize Automation Tasks:
    for( int i = 0; i < NUM_RELAY; i++){
      auto_dat[i] = {false, false, false, TEMP, 0, 0};  // Init Struct
    }

  
  // Load EEPROM DATA (Auth and Automation)
  auth_data auth_test;
      
  EEPROM.get(0, ee_test);
      
  if(ee_test == UNIQUE){
    EEPROM.get(sizeof(byte), auth_dat);  // Auth
    EEPROM.get(sizeof(byte) + sizeof(auth_data), auto_dat[0]);  // automation 1
    EEPROM.get(sizeof(byte) + sizeof(auth_data) + sizeof(auto_data), auto_dat[1]);  // automation 2 
  } 
  
  // Retain Variables
  while(1){  // Will need to exit this loop when power saving...
  
    // Read Data (and update automation):
     auto_data *autptr = &auto_dat[0];  // Pointer to "0" element

     
     
     
     checkSerial(autptr);    // Check for message, parse, and act.
    
    // Read Sensor Data and Update Struct:
    readSensors(&s_dat);  // Updates struct


    // Perform Automation Tasks:
    for( int i = 0; i < NUM_RELAY; i++){
      do_automation(i, &auto_dat[i], &s_dat);
    }
    
    // Report data if authenticated
    if(session_auth && auth_dat.disp_data){
      // Upload Sensor Data (if enabled)
      uploadSensors(&s_dat);
    }
    
    
    // Update EEPROM
    if(auth_dat.eeprom_update){
      
      // Update Status Bits:
      auth_dat.eeprom_update = 0;
      
      // Clear EEPROM:
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
      }
    
      // Store Auth and Automation Structs
      EEPROM.put(0, unique);
      EEPROM.put(sizeof(byte), auth_dat);  // Auth
      EEPROM.put(sizeof(byte) + sizeof(auth_data), auto_dat[0]);  // automation 1
      EEPROM.put(sizeof(byte) + sizeof(auth_data) + sizeof(auto_data), auto_dat[1]);  // automation 2 
    }
    
    // Call Break or Continue to leave this loop...
    // if(<expression>){
    //    break;
    // }
  
  }
}


//
// Functions
//


// Check for Incoming Serial Message
// Checks Serial Buffer and Calles Parser
void checkSerial(auto_data *auto_dat){
  
  char mbuffer[MAX_MSG_SIZE + 1];  // Incoming Message Buffer
  int i = 0;  // Index for Clearning Buffer
  
  
  if(Serial.available() > 0){
    Serial.readBytesUntil('\n', mbuffer, MAX_MSG_SIZE);
    
    // Call Parser:
    char * pntr = &mbuffer[0];
    if(parseMessage(pntr, MAX_MSG_SIZE, auto_dat)){
     
     // Confirm Understood 
     
    }
    else{
    // Bad Message:
        //Serial.print("Bad Message: ");
        //Serial.println(mbuffer);
        // Confirmation handeled by callee      
    }
    

    // Clear Message Buffer
    for(i = 0; i < MAX_MSG_SIZE; i++){
      mbuffer[i]= '\0';
    }
  }
  
}


// Incoming Message Parser
// Parses message buffer for defined RX Messages
// Respons using defined messages (blu_defs.h)
byte parseMessage(char * msg, int len, auto_data *auto_dat){
 
  
  // Serial.println(msg);
  // Vars:
  char channel[] = {'0', '\0'};
  char body[MAX_MSG_SIZE] = {'0', '\0'};
  
  // array [i] = * (array + i)
  byte i = 0, j = 0, sel = NUM_RX_MSG + 1;
  char buf[MAX_MSG_SIZE + 1];
 
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
        // Trim the string manually: 
        if(msg[i + MESSAGE_LEN] == '\n' || msg[i + MESSAGE_LEN] == '\r' || msg[i + MESSAGE_LEN] == '\0' || msg[i + MESSAGE_LEN] == ' '){
          buf[i]= '\0';
          break;
        } 
        buf[i] = msg[i + MESSAGE_LEN]; // BUF holds message body:
       }

       break;
     }
  }



  // Respond to message : rx_msg[j]
  switch (sel) {
      
      case 0:    // Automation Channel Select <Relay><Device>
      
              if(session_auth){
                channel[0] = buf[0];  // Must load single char into seperate buf for atoi
                body[0] = buf[1];
                (auto_dat + atoi(channel))->device = (sensor_sel)atoi(body);
                
                //auth_dat.eeprom_update = 1;
                return 1;  // Mark Succesfull 
              }
              else{
                Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
                return 1;
              }
        break;
        
      case 1:    // Automation Flag Set       <Relay><TRUE/FALSE>
          // Check True = Enabled, False = Disabled
  
          if(session_auth){
            channel[0] = buf[0];
            strncpy(body, buf + 1, 4);  // Extract only 4 Chars
            body[BODYSIZE] = '\0';
            if(!strcmp(body, M_FALSE)){
               (auto_dat + atoi(channel))->en = false;
               
               //auth_dat.eeprom_update = 1;
               return 1;
            }
            else if(!strcmp(body, M_TRUE)){
              (auto_dat + atoi(channel))->en = true;
              
              //auth_dat.eeprom_update = 1;
              return 1;
            }
            else{
              // Didn't Understand
              Serial.println(M_ERROR M_BAD);
              return 0;
            }
          }
          else{
            Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
            return 1;
          }
                  
        break;
      
      case 2:    // Automation Set Point      <RELAY><VALUE>
                 // Note atoi / atof return 0 if bad string
      
          if(session_auth){       
            channel[0] = buf[0];
            strncpy(body, buf + 1, MAX_MSG_SIZE - 1);
            (auto_dat + atoi(channel))->setpoint = atof(body);
            
            //auth_dat.eeprom_update = 1;             
            return 1;
          }
          else{
            Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
            return 1;
          }
          
        break;
      
      case 3:    // Automation Duration       <VALUE>

          if(session_auth){  
            channel[0] = buf[0];
            strncpy(body, buf + 1, MAX_MSG_SIZE - 1);
            (auto_dat + atoi(channel))->t_duration = atoi(body);
            
            //auth_dat.eeprom_update = 1;
            return 1;
          }
          else{
            Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
            return 1;
          }
      
        break;
      
      case 4:    // Automation Complete (conf) <VALUE> 
          if(session_auth){  
              // NOT USED //
              
          }
          else{
            Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
            return 1;
          }
                
        break;
      
      case 5:    // Configure Polling Freq.    <VALUE>
        if(session_auth){  
 
          auth_dat.poll_freq = (byte)atoi(buf);
          //auth_dat.eeprom_update = 1;
          return 1;
        }
        else{
          Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
          return 1;
        }
        break;
      
      case 6:    // Perform Data Operation (upload ready) <BODY>
        if(session_auth){ 
    
          buf[BODYSIZE] = '\0';
          if(!strcmp(buf, M_START)){
             auth_dat.disp_data = true;
             return 1;
          }
          
          if(!strcmp(buf, M_END)){
            auth_dat.disp_data = false;
            return 1;
          }

           // Didn't Understand
          Serial.println(M_ERROR M_BAD);
          return 0;

        }      
        else{
          Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
          return 1;
        }              
        break;
      
      case 7:    // Perform Status <BODY>
          buf[BODYSIZE] = '\0';
          if(session_auth){  
  
            if(!strcmp(buf, M_AUTOS)){  // Report Automation Status
              
              #ifdef DEBUG_EN
              if(auto_dat->en){
                Serial.println("Channel 0 Enabled");
                Serial.println((int)auto_dat->device);
                
              }
              if((auto_dat + 1)->en){
                Serial.println("Channel 1 Enabled");
                Serial.println((int)(auto_dat + 1)->device);
                
              }
              #endif
              
              return 1;  
            }
            
            if(!strcmp(buf, M_DATAS)){  // Report Data Status
             
              return 1; 
            }
            
            if(!strcmp(buf, M_AUTHS)){  // Report Authentication Status
             
             if(session_auth){
               Serial.println(M_AUTH M_TRUE);
             }
             else{
               Serial.println(M_AUTH M_FALSE);
             }
             
             return 1; 
            }
            
            Serial.println(M_HELLO M_TRUE);  // General Status Report
            return 1;
          }
          else{
            Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
            return 1;
          }
      
      break;
      
      case 8:    // Perform Auth <KEY> 
          return authenticate(atoi(buf));  
          break;
          
      case 9:   // Toggle Enable <Relay><T/>
               
        if(session_auth){

            channel[0] = buf[0];
            strncpy(body, buf + 1, 4);  // Extract only 4 Chars
            body[BODYSIZE] = '\0';
            if(!strcmp(body, M_FALSE)){
               (auto_dat + atoi(channel))->toggle = false;
               
               //auth_dat.eeprom_update = 1;
               return 1;
            }
            else if(!strcmp(body, M_TRUE)){
              (auto_dat + atoi(channel))->toggle = true;
              
              //auth_dat.eeprom_update = 1;
              return 1;
            }
            else{
              // Didn't Understand
              Serial.println(M_ERROR M_BAD);
              return 0;
            }
          }
          else{
            Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
            return 1;
          }
        
        break;
      
      case 10:      // Decending Value Enable Flag

        if(session_auth){
            channel[0] = buf[0];
            strncpy(body, buf + 1, 4);  // Extract only 4 Chars
            body[BODYSIZE] = '\0';
            if(!strcmp(body, M_FALSE)){
               (auto_dat + atoi(channel))->desc = false;
               
               //auth_dat.eeprom_update = 1;
               return 1;
            }
            else if(!strcmp(body, M_TRUE)){
              (auto_dat + atoi(channel))->desc = true;
              
              //auth_dat.eeprom_update = 1;
              return 1;
            }
            else{
              // Didn't Understand
              Serial.println(M_ERROR M_BAD);
              return 0;
            }
          }
          else{
            Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
            return 1;
          }
    
        break;  
        
         case 11:      // Write to EEPROM

        if(session_auth){
  
            strncpy(body, buf, 4);  // Extract only 4 Chars
            body[BODYSIZE] = '\0';
            if(!strcmp(body, M_TRUE)){
              auth_dat.eeprom_update = 1;
              return 1;
            }
            else{
              // Didn't Understand
              Serial.println(M_ERROR M_BAD);
              return 0;
            }
          }
          else{
            Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
            return 1;
          }
    
        break;  
        
      default:
          // Message parsed but not understood. 
          Serial.println(M_BADMSG M_BAD);
          return 0;
          break;
  }
  
  return 0;
}



// Read Data from Sensors:
// Updates struct via pointer
void readSensors(sensor_data * data){

   data->temp = bme.readTemperature();                // Temp in Celcius
   data->pressure = (bme.readPressure() / 100.0F);    // Pressure in hPa
   data->humidity = bme.readHumidity();               // Percent Realitive Humidity
   data->ls = analogRead(LIGHT_SENSOR);               // Light Level (10-bit)
   data->PIR = digitalRead(PIR0);                     // Active HIGH (indicates motion)   
}



// Authenticate Device:
// Checks auth_dat global structure
// Updates global "authenticated" bool
byte authenticate(int key){

  // No Key Set: Update
  if (auth_dat.auth_set == false){
    auth_dat.key = key;
    auth_dat.auth_set = true;
    session_auth = true;
    // Store AUTH STRUCT TO EEPROM //
    auth_dat.eeprom_update = 1;
    Serial.println(M_AUTH M_TRUE);  // Auth + True
    return 1;
  }
  
  // Key Set - Validate:
  else{
     
   if(auth_dat.key == key){
      session_auth = true;
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




// Do Automation:
// Requires the Use of a timer function using IRQ for Toggle
// Do not use delay - will cause sensor readings to not update!
// Will also require that automation structure is up-to-date
void do_automation(int relay, auto_data * auto_dat, sensor_data * sensor_dat ){
  if(auto_dat->en){  // Enable automation
    
    switch(auto_dat->device){
     case TEMP:  // Temperature
       if(auto_dat->desc){ // Equal or Lower
         if(sensor_dat->temp <= auto_dat->setpoint){
            enable_relay(relay, auto_dat->t_duration);
         }
         else{
            disable_relay(relay); 
         }
       }
       else{  // Equal or Greater
         if(sensor_dat->temp >= auto_dat->setpoint){
            enable_relay(relay, auto_dat->t_duration);
         }
         else{
            disable_relay(relay); 
         }
       }
     break;
     
     case PRES:  // Pressure
       if(auto_dat->desc){ // Equal or Lower
         if(sensor_dat->pressure <= auto_dat->setpoint){
            enable_relay(relay, auto_dat->t_duration);
         }
         else{
            disable_relay(relay); 
         }
       }
       else{  // Equal or Greater
         if(sensor_dat->pressure >= auto_dat->setpoint){
            enable_relay(relay, auto_dat->t_duration);
         }
         else{
            disable_relay(relay); 
         }
       }
     break;
     
     case HUMI:  // Humidity
       if(auto_dat->desc){ // Equal or Lower
         if(sensor_dat->humidity <= auto_dat->setpoint){
            enable_relay(relay, auto_dat->t_duration);
         }
         else{
            disable_relay(relay); 
         }
       }
       else{  // Equal or Greater
         if(sensor_dat->humidity >= auto_dat->setpoint){
            enable_relay(relay, auto_dat->t_duration);
         }
         else{
            disable_relay(relay); 
         }
       }
     break;
     
     case LIGHT:  // Light Sensor
       if(auto_dat->desc){ // Equal or Lower
         if(sensor_dat->ls <= auto_dat->setpoint){
            enable_relay(relay, auto_dat->t_duration);
         }
         else{
            disable_relay(relay); 
         }
       }
       else{  // Equal or Greater
         if(sensor_dat->ls >= auto_dat->setpoint){
            enable_relay(relay, auto_dat->t_duration);
         }
         else{
            disable_relay(relay); 
         }
       }
     break;
     
     case PIR: // Motion Sensor (May Require Some Debugging like a counter)
       if(sensor_dat->PIR){
           enable_relay(relay, auto_dat->t_duration);
       }
       else{
           disable_relay(relay);
       }
    }
    
  }
  
  else{    // Automation Disabled
   
     disable_relay(relay); 
  }
}



// Called to shut a specified relay off (Pulse Reset)
void enable_relay(int relay, int toggle){
  relay_tog = relay;
  // Turn ON Relay 
  switch (relay){
    
    #ifdef DEBUG_EN
      case 0:
        digitalWrite(R0_SET, HIGH);
        break;
      case 1:
        digitalWrite(R1_SET, HIGH);
        break;
      default:
        digitalWrite(R0_SET, HIGH);
        digitalWrite(R1_SET, HIGH);
        break;
    
    #else
    
    case 0: // Relay 0
      digitalWrite(R0_SET, HIGH);
      delay(RELAY_PW_DELAY);
      digitalWrite(R0_SET, LOW);
      break;
      
    case 1: // Relay 1
      digitalWrite(R1_SET, HIGH);
      delay(RELAY_PW_DELAY);
      digitalWrite(R1_SET, LOW);
      break;
      
    default: // Both Relays
      digitalWrite(R0_SET, HIGH);
      digitalWrite(R1_SET, HIGH);
      delay(RELAY_PW_DELAY);
      digitalWrite(R0_SET, LOW);
      digitalWrite(R1_SET, LOW);
      break;
  #endif
  } 
  
  
 
 // Toggle Off after Duration
 if(toggle > 0){
  
    // Delay
    //relaytimer(toggle);
    delay(toggle);  // This is going to cause problems!
    disable_relay(relay); // Turn Off
 
  } 
  
}

void disable_relay(int relay){
   // Turn Off This Relay
  switch (relay){
    
    #ifdef DEBUG_EN
      case 0:
        digitalWrite(R0_SET, LOW);
        break;
      case 1:
        digitalWrite(R1_SET, LOW);
        break;
      default:
        digitalWrite(R0_SET, LOW);
        digitalWrite(R1_SET, LOW);
        break;
    
    #else
    
    case 0:  // Relay 0
      digitalWrite(R0_RSET, HIGH);
      delay(RELAY_PW_DELAY);
      digitalWrite(R0_RSET, LOW);
      break;
      
    case 1:  // Relay 1
      digitalWrite(R1_RSET, HIGH);
      delay(RELAY_PW_DELAY);
      digitalWrite(R1_RSET, LOW);
      break;
       
    default:  // Both Relays
      digitalWrite(R0_RSET, HIGH);
      digitalWrite(R1_RSET, HIGH);
      delay(RELAY_PW_DELAY);
      digitalWrite(R0_RSET, LOW);
      digitalWrite(R1_RSET, LOW);
      break;
   #endif
  }
}


// Upload Sensor Data:
// Prints Values using defined serial messages
void uploadSensors(sensor_data * s_dat){
  
  // SEND SENSOR DATA HERE //
  Serial.print(M_TEMP + String(s_dat->temp, PRECISION) + "\n");        // Temperature
  delay(20);
  
  Serial.print(M_PRESS + String(s_dat->pressure, PRECISION) + "\n");   // Pressure
  delay(20);
  
  Serial.print(M_HUMID + String(s_dat->humidity, PRECISION) + "\n");   // Humidity
  delay(20);
  
  Serial.print(M_PHOTO + String(s_dat->ls) + "\n");                    // Photo Sensor
  delay(20);
  
  if(s_dat->PIR){    // Motion Sensor State
   Serial.print(M_PIR M_TRUE "\n");
  }
  else {
    Serial.print(M_PIR M_FALSE "\n");
  }
  
  delay(1000); // FOR DEBUGGING? Might use polling delay here somehow...
}
