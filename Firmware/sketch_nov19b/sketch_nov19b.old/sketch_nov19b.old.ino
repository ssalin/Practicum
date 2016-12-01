#include <Arduino.h>  // Arduino Type Definitions
#include <EEPROM.h>   // EEPROM Library
#include <avr/wdt.h> 
#include "blu_defs.h"   // Macros & Prototypes

unsigned long x=0;
// Global Vars:
auth_data auth_dat;     // Auth Data Struct - Note that using pointers caused weird problems...
sensor_data sensor_dat; // Sensor Data Struct
auto_data auto_dat[NUM_RELAY];   // Automation Data
volatile bool Run = false;        // ISR Flag
volatile int wSetting;        //variable to store WDT setting, make it volatile since it will be used in ISR

void setup()
{
  Serial.begin(9600);
  auth_dat = {false,0,0};
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
  
  EEPROM.put(AUTH_ADDR, auth_dat);
  EEPROM.put(AUTO_ADDR, auto_dat[0]);
  EEPROM.put(AUTO_ADDR + AUTO_SIZE, auto_dat[1]);

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

// This funciton will create new content for the struct object and then puts it in the specified memory location
void put(int address, int index)
{
  byte num = random(63);
  auto_dat[index].en = bitRead(num,0);   // Enable
  auto_dat[index].dec = bitRead(num,1);   // Descending set point
  auto_dat[index].tog = bitRead(num,2);   // Toggle Relay
  auto_dat[index].tmp = bitRead(num,3);  // Temperature Sensor
  auto_dat[index].pres = bitRead(num,4);  // Pressure Sensor
  auto_dat[index].hum = bitRead(num,5);  // Humidity sensor
  auto_dat[index].ls = bitRead(num,6);    // Light sensor
  auto_dat[index].pir = bitRead(num,7);   // PIR Motion Sensor
    
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
  Serial.println( foo.dec );      
  Serial.println( foo.tog );
  Serial.println( foo.tmp );
  Serial.println( foo.pres );
  Serial.println( foo.hum );
  Serial.println( foo.ls );
  Serial.println( foo.pir );
  Serial.println( foo.setpoint );
  Serial.println( foo.t_duration );
  
}


void loop()
{
  if (Run){
      cli();
      get(AUTO_ADDR);
      delay(1000);
      get(AUTO_ADDR + AUTO_SIZE);
      delay (1000);
      put(AUTO_ADDR, 1);
      delay (1000);
      put(AUTO_ADDR + AUTO_SIZE, 0);
      Run = false;
      sei();
  }
  
  // Resetting the timer in less than 4sec will prevent the interrupt or system reset.
  // Commenting out the reset will cause either an interrupt or system reset.
  // System reset will cause x to start back at 1, while an interrupt will
  // simply display the "INTERRUPT!!!" message and continue the loop where it left off.
  //wdt_reset();
}

ISR(WDT_vect)
{
  Run = true;
  Serial.println("INTERRUPT!!!");
}
