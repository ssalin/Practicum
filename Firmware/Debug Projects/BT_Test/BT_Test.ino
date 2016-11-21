/*
*
*  BT Serial Test
*
*
*/

#include "blu_defs.h"

auth_data auth_dat;              // Auth Data

#define BUF 1024
#define MAX_MSG_SIZE 18 // 18 Chars Max

bool device_authenticated = false;
char mbuffer[BUF];
int i = 0;


void setup() {
  
  auth_dat = {false,0,0};   // Init Auth Struct
  Serial.begin(9600);       // Enable Serial
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
  if(Serial.available() > 0){
    Serial.readBytesUntil('\n', mbuffer, BUF);
    
    // Call Parser:
    char * pntr = &mbuffer[0];
    if(parseMessage(pntr, BUF)){
     
     // Confirm Understood 
     
     // Do something  
    }
    else{
    // Bad Message:
      Serial.println(M_BADMSG M_BAD);
    }
    
    // Clear Message Buffer
    for(i = 0; i < BUF; i++){
      mbuffer[i]= '\0';
    }
  }

} // END LOOP //

// Incoming Message Parser
// Uses global message buffer (may be bad)
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
       continue;
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
          return authenticate(atoi(buf));  
          break;
       
      default:
          // Message parsed but not understood. 
          return 0;
          break;
  }
  
  return 0;
}

// Device Functions:
  
  
// Authenticate Device:
byte authenticate(int key){

  // Set Passcode:
  if (auth_dat.auth_set == false){
    auth_dat.key = key;
    auth_dat.auth_set = true;
    device_authenticated = true;
    // Store AUTH STRUCT TO EEPROM //
    Serial.println(M_AUTH M_TRUE);  // Auth + True
    return 1;
  }
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
