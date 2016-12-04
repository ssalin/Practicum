/*
 * 
 * BluDaq Firmware V0.1
 * Nov 2016
 * 
 */


#include "blu_defs.h"         // Macros & Prototypes
#include <Arduino.h>          // Arduino Type Definitions
#include <EEPROM.h>           // EEPROM Library
#include <Wire.h>             // I2C Library
#include <SPI.h>              // SPI Library (needed for class)
#include <Adafruit_Sensor.h>  // BME280 
#include <Adafruit_BME280.h>  // BME280


// Globals:

Adafruit_BME280 bme;           // I2C BME280 Instance
bool run_once = false;         // Mark Reboot Status
auth_data auth_dat;            // Authentication Token
bool session_auth = false;     // Session Authenticated

// Setup Loop:
void setup() {
  
  // Init Serial
  Serial.begin(BAUD_RATE);
  
  // Init BME280
  if (!bme.begin()) {
    // Report Error with I2C Connection to BME280
    Serial.println(M_ERROR M_BME280);

  }
  
  // Config Auth Token
  auth_dat = {false, false, 0, 0};
}



// Main Loop:
void loop() {
  
  // FIGURE OUT WATCHDOG STUFF HERE ? //
  
  // Local Variables:
  sensor_data s_dat = {false, 0, 0, 0, 0};   // Sensor Data Struct
  auto_data auto_dat[NUM_RELAY];             // Automation Structures 

  // Initialize Automation Tasks:
    for( int i = 0; i < NUM_RELAY; i++){
      auto_dat[i] = {false, false, false, TEMP, 0, 0};  // Init Struct
    }

  
  // Load EEPROM DATA (Auth and Automation)
  if(!run_once){
        
    // DO EEPROM STUFF HERE //  
    
    run_once = true; 
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
        buf[i] = msg[i + MESSAGE_LEN]; // BUF holds message body:
       }
       
       buf[i]= '\0';
       //buf.trim();
       //continue;
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
                 return 1;  // Mark Succesfull 
              }
              else{
                Serial.println(M_AUTH M_NOAUTH); // Not Authenticated
                return 1;
              }
        break;
        
      case 1:    // Automation Flag Set       <Relay><TRUE/FALSE>
          // Check True = Enabled, False = Disabled
          buf[BODYSIZE] = '\0';
          if(session_auth){
            channel[0] = buf[0];
            strncpy(body, buf + 1, 4);  // Extract only 4 Chars
            if(!strcmp(body, M_FALSE)){
               (auto_dat + atoi(channel))->en = false;
               return 1;
            }
            else if(!strcmp(body, M_TRUE)){
              (auto_dat + atoi(channel))->en = true;
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
       
      default:
          // Message parsed but not understood. 
          return 0;
          break;
  }
  
  return 0;
}



// Read Data from Sensors:
void readSensors(sensor_data * data){

   data->temp = bme.readTemperature();                // Temp in Celcius
   data->pressure = (bme.readPressure() / 100.0F);    // Pressure in hPa
   data->humidity = bme.readHumidity();               // Percent Realitive Humidity
   data->ls = analogRead(LIGHT_SENSOR);               // Light Level (10-bit)
   data->PIR = digitalRead(PIR0);                    // Active LOW (indicates motion)?
   
   #ifdef DEBUG_EN
     Serial.println("DATA BEGIN:");
     Serial.println(data->temp);
     Serial.println(data->pressure);
     Serial.println(data->humidity);
     Serial.println(data->ls);
     Serial.println(data->PIR);
     delay(1000);
   #endif
   
   
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
  
  // Turn ON Relay 
  switch (relay){
    case 0: // Relay 0
      digitalWrite(R0_SET, HIGH);
      delay(RELAY_PW_DELAY);
    //  digitalWrite(R0_SET, LOW);
      break;
      
    case 1: // Relay 1
      digitalWrite(R1_SET, HIGH);
      delay(RELAY_PW_DELAY);
   //   digitalWrite(R1_SET, LOW);
      break;
      
    default: // Both Relays
      digitalWrite(R0_SET, HIGH);
      digitalWrite(R1_SET, HIGH);
      delay(RELAY_PW_DELAY);
      digitalWrite(R0_SET, LOW);
      digitalWrite(R1_SET, LOW);
      break;
  } 
 
 // Toggle Off after Duration
 if(toggle > 0){
  
    // Delay
    delay(toggle);  // This is going to cause problems!
    disable_relay(relay); // Turn Off
 
  } 
  
}

void disable_relay(int relay){
   // Turn Off This Relay
  switch (relay){
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
  }
}



// Upload Sensor Data:
// Prints Values using defined serial messages
void uploadSensors(sensor_data * s_dat){
  
  // SEND SENSOR DATA HERE //
  Serial.println(M_TEMP + String(s_dat->temp, PRECISION));        // Temperature
  Serial.println(M_PRESS + String(s_dat->pressure, PRECISION));   // Pressure
  Serial.println(M_HUMID + String(s_dat->humidity, PRECISION));   // Humidity
  Serial.println(M_PHOTO + String(s_dat->ls));                    // Photo Sensor
  
  if(s_dat->PIR){    // Motion Sensor State
   Serial.println(M_PIR M_TRUE);
  }
  else {
    Serial.println(M_PIR M_FALSE);
  }
  
  delay(1000); // FOR DEBUGGING? Might use polling delay here somehow...
}
