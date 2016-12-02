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
volatile bool Run = false;        // ISR Flag
//volatile int wSetting;        //variable to store WDT setting, make it volatile since it will be used in ISR
volatile int count = 0;
auto_data auto_dat[NUM_RELAY];             // Automation Structures 

// Setup Loop:
void setup() {
  
  // Init Serial
  Serial.begin(BAUD_RATE);
  
  // Init BME280
  if (!bme.begin()) {
    // Send Error & Reboot

  }
  
  // Config Auth Token
  auth_dat = {false, 0, 0};

#ifdef DEBUG_EN
  // WatchDog Timer Interrupt
  // Disable interrupts globally
  cli();
  
  // Reset Watchdog Timer
  MCUSR &= ~(1<<WDRF);  
  
  // Prepare watchdog for config change.
  WDTCSR |= (1<<WDCE) | (1<<WDE);  
  
  // Now choose timer mode and prescaler
  WDTCSR = (1<<WDIE) | (1<<WDP3) | (1<<WDP0);  // 8sec, Interrupt Mode
 
  // Enable interrupts globally
  sei();

}

ISR(WDT_vect)
{
  Run = true;
  Serial.println("INTERRUPT!!!");
#endif
}


// Main Loop:
void loop(){
  if (Run){
    
    cli();      // Turn off global interrupts
    Serial.println( "Interrupt disabled" );

    // Local Variables:
    sensor_data s_dat = {false, 0, 0, 0, 0};   // Sensor Data Struct
//    auto_data auto_dat[NUM_RELAY];             // Automation Structures 
//    auto_data temp;
//  
//    Serial.println(sizeof(temp));
//    Serial.println(sizeof(auto_dat));
    
    // Load EEPROM DATA (Auth and Automation)
    if(!run_once){
          
      // DO EEPROM STUFF HERE //  
      EEPROM.put(0, auth_dat);
      EEPROM.put(sizeof(auth_data), auto_dat[0]);
      EEPROM.put(sizeof(auth_data) + sizeof(auto_data), auto_dat[1]);
  
      run_once = true; 
    }
    
#ifdef DEBUG_EN
    // Debug code
    if ( count % 3 == 0){
      put(sizeof(auth_data),0);
      put(sizeof(auth_data) + sizeof(auto_data),1);
    }
    else{
      get(sizeof(auth_data));
      get(sizeof(auth_data) + sizeof(auto_data));
    }
#endif    
    // Read Data:
     checkSerial();    // Check for message, parse, and act.
    
    // Read Sensor Data and Update Struct:
    readSensors(&s_dat);  // Updates struct
    
    // Perform Automation Tasks:
    for( int i = 0; i < NUM_RELAY; i++){
      auto_dat[i] = {false, false, false, TEMP, 0, 0};  // Init Struct
      do_automation(i, &auto_dat[i], &s_dat);
    }
    
    // Report data if authenticated
    if(session_auth){
      // Upload Sensor Data (if enabled)
      uploadSensors(&s_dat);
    }
    
    count += 1;
    Serial.println(count);
    Run = false;
    Serial.println( "Interrupt re-enabled" );
    
    sei();      // Turn interrupt back on

  }
}


//
// Functions
//
// This funciton will create new content for the struct object and then puts it in the specified memory location
void put(int address, int index)
{
//  byte num = random(63);
  auto_dat[index].en = false;   // Enable
  auto_dat[index].desc = true;   // Descending set point
  auto_dat[index].toggle = false;   // Toggle Relay
//  auto_dat[index].tmp = bitRead(num,3);  // Temperature Sensor
//  auto_dat[index].pres = bitRead(num,4);  // Pressure Sensor
//  auto_dat[index].hum = bitRead(num,5);  // Humidity sensor
//  auto_dat[index].ls = bitRead(num,6);    // Light sensor
//  auto_dat[index].pir = bitRead(num,7);   // PIR Motion Sensor
//    
  auto_dat[index].setpoint = random(100);   // Setpoint 
  auto_dat[index].t_duration = random(100);   // Toggle Duration

  EEPROM.put(address, auto_dat[index]);
}

// This funciton will create a new object and get the information from the specified memory address 
void get(int address)
{
  auto_data foo;
  EEPROM.get(address, foo);
  Serial.print(F("==========================\n"));
  Serial.println( foo.en ); 
  Serial.println( foo.desc );      
  Serial.println( foo.toggle );
//  Serial.println( foo.tmp );
//  Serial.println( foo.pres );
//  Serial.println( foo.hum );
//  Serial.println( foo.ls );
//  Serial.println( foo.pir );
  Serial.println( foo.setpoint );
  Serial.println( foo.t_duration );
  
}




// Check for Incoming Serial Message
// Checks Serial Buffer and Calles Parser
void checkSerial(){
  
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
  
}


// Incoming Message Parser
// Parses message buffer for defined RX Messages
// Respons using defined messages (blu_defs.h)
byte parseMessage(char * msg, int len){
 
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
       //continue;
       break;
     }
  }
  Serial.println(buf);
  Serial.println(buf[0]);
  // Respond to message : rx_msg[j]
  switch (sel) {
      
      case 0:    // Automation Channel Select <Relay : Device>
       #ifdef DEBUG_EN
          if (atoi(buf) == 1){
              Serial.println(M_RL1 M_TRUE);
              return 1;
          }
          else{
              Serial.println(M_RL0 M_TRUE);
              return 1;
          }
      #endif
        break;
        
      case 1:    // Automation Flag Set       <VALUE>
        #ifdef DEBUG_EN
              Serial.println(buf);
              Serial.println(buf[0]);
              int rel = buf[0];
              char buff[MESSAGE_LEN];
              for(i = 1; i < MAX_MSG_SIZE; i++){
                  buff[i-1] = buf[i]; // BUFF holds message body:
              }
              buff[i]= '\0';
              Serial.println(buff);
          return auto_flag_set(atoi(rel), atoi(buff)); // or strtof() for string to float
        #endif
        break;
      
      case 2:    // Automation Set Point      <VALUE>
          #ifdef DEBUG_EN
              Serial.println(buf);
              Serial.println(buf[0]);
              int rel = buf[0];
              char buff[MESSAGE_LEN];
              for(i = 1; i < MAX_MSG_SIZE; i++){
                  buff[i-1] = buf[i]; // BUFF holds message body:
              }
              buff[i]= '\0';
              Serial.println(buff);
          return auto_duration(atoi(rel), atof(buff)); // or strtof() for string to float
          #endif
        break;
      
      case 3:    // Automation Duration       <VALUE>
        #ifdef DEBUG_EN
            Serial.println(buf);
            Serial.println(buf[0]);
            int rel = buf[0];
            char buff[MESSAGE_LEN];
            for(i = 1; i < MAX_MSG_SIZE; i++){
                buff[i-1] = buf[i]; // BUFF holds message body:
            }
            buff[i]= '\0';
            Serial.println(buff);
        return auto_duration(atoi(rel), atoi(buff));
        #endif
        break;
      
      case 4:    // Automation Complete (conf) <VALUE>
      
        break;
      
      case 5:    // Configure Polling Freq.    <VALUE>
        return config_poll_freq(atoi(buf));
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

#ifdef DEBUG_EN
// set automation flag, 
byte auto_flag_set(int rel, byte value){
  // Select relay
  auto_dat[rel].en = bitRead(value,0);
  auto_dat[rel].dec = bitRead(value,1);
  auto_dat[rel].tog = bitRead(value,2);
  auto_dat[rel].tmp = bitRead(value,3);         // Reading these might be tricky
  auto_dat[rel].pres = bitRead(value,4);
  auto_dat[rel].hum = bitRead(value,5);
  auto_dat[rel].ls = bitRead(value,6);
  auto_dat[rel].pir = bitRead(value,7);
  Serial.println(RM_AUTOFLAG M_TRUE);
  return 1;
}
#endif


#ifdef DEBUG_EN
// set automation set point
byte auto_set_point(int rel,float value){
  auto_dat[rel].setpoint = value;
  Serial.println(RM_AUTOSET M_TRUE);
  return 1;
}
#endif

// Automation struct Channel selection
#ifdef DEBUG_EN
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



// set automation duration
byte auto_duration(int rel, int value){
  // Select relay
  auto_dat[rel].t_duration = value;
  Serial.println(RM_AUTODUR M_TRUE);
  return 1;
}

#endif

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

// Read Data from Sensors:
void readSensors(sensor_data * data){

   data->temp = bme.readTemperature();                // Temp in Celcius
   data->pressure = (bme.readPressure() / 100.0F);    // Pressure in hPa
   data->humidity = bme.readHumidity();               // Percent Realitive Humidity
   data->ls = analogRead(LIGHT_SENSOR);               // Light Level (10-bit)
   data->PIR = !digitalRead(PIR0);                    // Active LOW (indicates motion)
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
  #ifdef DEBUG_EN
//  Serial.println(M_PHOTO s_dat->ls);
//  Serial.println(M_PIR s_dat->PIR);
//  Serial.println(M_TEMP s_dat->temp);
//  Serial.println(M_PRESS s_dat->pressure);
//  Serial.println(M_HUMID s_dat->humidity);
  #endif
}
