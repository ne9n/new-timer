/*
    added comments

*/

#include "led.h"
#include <Ramp.h> 
rampInt upRamp; 




int startTime =0 ;
int deltaTime = 0;

void speedState()
{
  deltaTime = millis() - startTime;
  esc.write(curThrottle);
  switch (fly_state)
  {
    case WAIT:
      /*
        Waits here until the start button is pressed
         Entrance -  Start up or land
        Exit Armed state
      */
      {
        led3.flashS();
        led4.off();
        led5.off();
        if (/*Sw1.isPressed()*/ digitalRead(SW1) == 0) // fix this later bitton method doesnt clear latch
        {
          curThrottle = 0;
          fly_state = ARMED;  
          startTime = millis();
          
        }
        break;
      }

    case ARMED:
      {
        /* waits here to walk out to the model
          Entry from wait state
          Exit timer expired and on to take off */
        led3.flashF();
        led4.flashF();
        led5.off();
    
        if (deltaTime > (cheze.ArmTime*1000) )
        {
         
          Serial.print ("\t armed \t");
          fly_state = TAKEOFF;
          curThrottle = 0;
          upRamp.go(0);   // set value to directly to 5000
          upRamp.go(cheze.FlySpeed,cheze.accelTimeMs);
        
        }
        break;
      }
    case TAKEOFF:
      {
        /*This is a complicated state
          this will ramp up speed to the base trottle position based on acclertion time
          Doe to the low resoultions, I may be off by one tick */
        /* inc or dec each time by Max speed (180)/acell time
          but that is less than so it wont work so one speed tic = accel time *1000/180
          so wait that long and add a tic each time though */
        led3.off();
        led4.flashF();
        led5.off();
        curThrottle= upRamp.update();
        if (curThrottle >= cheze.FlySpeed)
        {
          fly_state = FLY;
          startTime = millis();
        }
        break;
      }
    case FLY:
      {
        /* this has it own substates
          Flying-- can refualte speed based on state vector
          Burp-  step speed up as a warning to end of flight time
          rdyland wait some delay before landing */
        /* entry-- takeoff state
          Exit substate rdy land */
        led3.off();
        led4.flashS();
        led5.flashS();
        curThrottle = cheze.FlySpeed + posTrim;
        if (deltaTime > (cheze.FlyTime*1000) )
        {
          startTime= millis();
          fly_state = BURP;
        }
        break;
      }
    case BURP:
    {
      curThrottle = MAXT;
        led3.off();
        led4.flashF();
        led5.flashF();

      if (deltaTime > BURP_TIME )
      {
         startTime= millis();
         fly_state = BURP_DELAY;
         curThrottle = cheze.FlySpeed;
      }
      break;
    }
    case BURP_DELAY:
    {
        led3.flashF();
        led4.flashF();
        led5.flashF();
      if (deltaTime > BURP_WAIT)
      {
        upRamp.go(curThrottle);    
        upRamp.go(0,cheze.accelTimeMs);
         
          fly_state = RAMPDWN;
      }
      break;
    }
    case RAMPDWN:
      {
        led3.on();
        led4.off();
        led5.flashF();

        curThrottle= upRamp.update();
        if (curThrottle == 0)
        {
          fly_state = WAIT;
          curThrottle = 0;
        }
        break;
      }
  }

}
