#include "cheesehead_timer.h"
#include "led.h"
#include <Arduino.h>
extern speed_state ispeed_state;
//extern ezLED rled(LED5);
//extern ezLED yled(LED4);
//extern ezLED gled(LED3);

ezLED rled(LED5);
ezLED yled(LED4);
ezLED gled(LED3);


void initLED()
{
  gled.blinkNumberOfTimes(5000,  1000, 10);
  delay(1000);
  yled.blinkNumberOfTimes(5000, 1000, 10);
  
  delay(1000);
  yled.blinkNumberOfTimes( 5000, 1000, 10);


  rled.blink(500,500);
  Serial.print("LED init \n");

  
}

void updateLED()
{
  rled.loop();
  yled.loop();
  gled.loop();
  
  
}


void set_led(int is)
{

  delay(1000);
  

  switch (is)
  {
    case WAIT:
    {
          rled.blink(250,1000);
          yled.turnOFF();
          gled.turnOFF();

    }
        break;
    case ARMED:
    {
          rled.turnOFF();
          yled.blink(250,1000);
          gled.turnOFF();
    }
    break;
    case TAKEOFF_RAMP:
    {
          rled.turnOFF();
          yled.turnON();
          gled.blink(250,100);

        
    }
    break;
    case TAKEOFF:
    {
        rled.turnOFF();
        yled.turnOFF();
        gled.turnON();
    }
    break;
    case FLY:
    {
        rled.turnOFF();
        yled.blink(500,500);
        gled.blink(500,500);
    }
    break;
    case BURP:
  {
        rled.turnON();
        yled.turnON();
        gled.turnON();

    }
    break;

    case RDYLAND:
    {

    }
    break;
    case RAMPDWN:
    {

        rled.blink(250,1000);
          yled.turnOFF();
          gled.turnOFF();
       
    }
    break;
    default:
    {
        rled.turnON();
        yled.turnOFF();
        gled.turnOFF();
       
      
    }
    break;
  }
  return; 
}
