#include <TimerOne.h>

bool ISR = false;
volatile unsigned long count = 0;

void setup(void)
{

  Timer1.initialize(327680000);   //32.758kHz 
  Timer1.attachInterrupt(INTR);   // Interrupts and execute INTR function
  Serial.begin(9600);
}


// The interrupt will check the ISR status, 
void INTR(void)
{
   if (count < 10 || ISR == false){
      ISR = true;     // Flip ISR value to enter into main loop
      loop();
      count = count + 1;
   }
   else if (count == 10)
   {
      ISR = false;
      count = 0; 
   }
}

void loop(void)
{
  if (ISR == true)
  {
      Serial.print( "ISR status is " );
      Serial.println(ISR);
      noInterrupts(); //disable interrupt
      delay(3000);
      interrupts(); //re-enable interrupt
      Serial.print( "The interrupt has been disabled" );
  }
  else
  {
      Serial.print(" ISR status is ");
      Serial.println(ISR);
      Serial.print( "The interrupt has been re-enabled again" );
  }        
}
