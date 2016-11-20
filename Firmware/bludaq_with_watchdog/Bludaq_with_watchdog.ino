/*
 * 
 * BluDaq Firmware V0.2
 * Nov 2016
 * 
 */

#include "blu_defs.h"   // Macros & Prototypes
#include <Arduino.h>  // Arduino Type Definitions
#include <EEPROM.h>   // EEPROM Library
#include <Wire.h>     // I2C Library
#include <avr/wdt.h>  // Watchdog library


// Global Vars:
bool EEPROM_LD = false;           // EEPROM Load Flag
volatile bool Run = false;        // ISR Flag
auth_data *auth_dat;              // Auth Data
sensor_data *sensors_dat;         // Sensor Data
auto_data *auto_dat[NUM_RELAY];   // Automation Data
int default_delay = 327680000;    // deafult delay in ms 
int f_delay;                      // variable used to pass delay
volatile int wSetting = 9;        //variable to store WDT setting, make it volatile since it will be used in ISR


// EEPROM Function Example:
// EEPROM.put(address, *foo);
// EEPROM.get(address, foo);
// cli();       // Disable all interrupts
// sei();       // Enable all interrupts


void setup(){
  
  // Constructor for all global structs, only do this once. 
  *auth_dat = {false,0,0.0,0.0};
  *sensors_dat = {0,false,0.0,0.0,0.0};
  for (int i = 0; i < NUM_RELAY; i++){
    auto_data_constructor(auto_dat[i]);
    if (DEBUG_EN){                // Print when in debug mode
      Serial.print(F(" ======================== "));
      Serial.print(sizeof(*auto_dat[i]));
    }
  }

  // +++++++++++++++++++++++++++++++++++++++++
  // +++++++++++++++++++++++++++++++++++++++++
  // Unsure of how does the frequency relates to prescalar value. 
  // get timer value from auth.delay (polling frequency),
  EEPROM.get(AUTH_ADDR,*auth_dat);
  if (auth_data.poll_freq == 0.0){ 
      f_delay = default_delay;
  }
  else{
      f_delay = auth_data.poll_freq * 1000    // Result need to be in microseconds
  }

  // Disable interrupts globally
  cli(); 
  // Reset Watchdog Timer
  MCUSR &= ~(1<<WDRF);  
  // Prepare watchdog for config change.
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  // Prompt user for oscillator and choose timer mode and prescaler
  getSettings();
  if(wSetting == 1) WDTCSR = (1<<WDIE); //set for interrupt 16 ms
  else if(wSetting == 2) WDTCSR = (1<<WDIE) | (1<<WDP0); //set for interrupt 32 ms
  else if(wSetting == 3) WDTCSR = (1<<WDIE) | (1<<WDP1); //set for interrupt 64 ms
  else if(wSetting == 4) WDTCSR = (1<<WDIE) | (1<<WDP1) | (1<<WDP0); //set for interrupt 0.125 s
  else if(wSetting == 5) WDTCSR = (1<<WDIE) | (1<<WDP2); //set for interrupt 0.25 s
  else if(wSetting == 6) WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP0); //set for interrupt 0.5 s
  else if(wSetting == 7) WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1); //set for interrupt 1 s
  else if(wSetting == 8) WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0); //set for interrupt 2 s
  else if(wSetting == 9) WDTCSR = (1<<WDIE) | (1<<WDP3); //set for interrupt 4 s
  else if(wSetting == 10) WDTCSR = (1<<WDIE) | (1<<WDP3) | (1<<WDP0); //set for interrupt 8 s
    
  //WDTCSR = (1<<WDE) | (1<<WDP3);  // 4sec, System Reset Mode
  //WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);  // 2sec, Interrupt Mode
  //WDTCSR = (1<<WDE) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);  // 2sec, System Reset Mode 
   
  // Enable interrupts globally
  sei();  
} 


//This function gets the WDT setting from the user using Serial comm
void getSettings() {
  Serial.begin(9600);
  Serial.println("Select the Oscillator setting: 1 --> 16 ms, 2 --> 32 ms, 3 --> 64 ms");
  Serial.println("Select the Oscillator setting: 4 --> 0.125 s, 5 --> 0.25 s, 6 --> 0.5 ms");
  Serial.println("Select the Oscillator setting: 7 --> 1 s, 8 --> 2 s, 9 --> 4 s, 10 --> 8 s");
  //wait for user to input setting selection
  while (!Serial.available()) { }
  wSetting = Serial.parseInt(); //get entry and ensure it is valid
  if(wSetting < 1 || wSetting > 10) {
      wSetting = 9;
   }
   Serial.print("WDT setting is ");
   Serial.println(wSetting);
   Serial.println();
   Serial.end(); //don't want to mix serial comm and interrupts together, serial communication uses interrupt and don't want to mix with watchdog
}


// Main Loop
void loop() {
  
  /* Empty loop */
  if (Run){
      // cli();                //disable interrupt

      // Test EEPROM Flag
      if (EEPROM_LD == false){
          // Load EEPROM
          // auth_data authen;              // Auth Data
          // auto_data relay1,relay2;         // Sensor Data
          // EEPROM.get(0, authen);
          // EEPROM.get(12, relay1);
          // EEPROM.get(19, relay2);
          EEPROM_LD = true;  
          // call Store to Automation Struct
      }
      
      // If (automation){
            // read_store_sensordata()  //Read Sensor Data and store sensor to struct
    
      // }
      
      // If (Serial){
              // if (Host is not Authenticated){
                  // Return not authorized
              // }

              // else{
                    // if (new automation){
                        // call store to EEPROM 
                        // Update automation struct 
                    // }
            
            
                 // upload sensor data to BT device
              // }
            
         Serial.flush()
      // }
      
      
      // Check for automation, do automation if found
      }
      Run = false;
      // sei();                  //re-enable interrupt
  }
}

// Read and store sensor data struct
void read_store_sensordata(void){
    // Read from sensors and store in sensors_dat struct
    *sensors_dat->ls = 8;
    *sensors_dat->PIR = true;
    *sensors_dat->temp = 79.9;
    *sensors_dat->pressure = 23.9;
    *sensors_dat->humidity = 43.0;
}

// Reset sensor data struct
void reset_sensordata(void){
    *sensors_dat->ls = 0;
    *sensors_dat->PIR = false;
    *sensors_dat->temp = 0.0;
    *sensors_dat->pressure = 0.0;
    *sensors_dat->humidity = 0.0;
  
}


// Function to update EEPROM automation struct
void store_EEPROM(){


}


// Load sensor data
void upload_sensordata(){
  
  
}



// Interrupt vector, change flag status for main loop
ISR(WDT_vect)
{
  Run = true;
  Serial.println("INTERRUPT!!!");
}


/*  Power saving, timer value is the time watchdog waits up from sleep
void setSleepInterval(byte timer) {
  sleep_enable(); //enable the sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); //set the type of sleep mode. Default is Idle. power down saves the most power
  ADCSRA &= ~(1<<ADEN); //Turn off ADC before going to sleep (set ADEN bit to 0). this saves even more power
  WDTCSR |= 0b00011000;    //Set the WDE bit and then clear it when set the prescaler, WDCE bit must be set if changing WDE bit   
  WDTCSR = 0b01000000 | timer; //This sets the WDT to the interval specified in the function argument
  wdt_reset(); //Reset the WDT 
  sleep_cpu(); //enter sleep mode. Next code that will be executed is the ISR when interrupt wakes Arduino from sleep
  sleep_disable(); //disable sleep mode
  ADCSRA |= (1<<ADEN); //Turn the ADC back on
}*/
