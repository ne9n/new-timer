/*
 *  added comments
 *  
 */

#include "led.h"



void flyState()
{
 
  switch (inFlight)
  {
    case FLYING:
      {
        curThrottle = cheze.FlySpeed + posTrim;
        if ( (currentMillis- state_tmr) > cheze.FlyTime*1000 )
        {
           //fly_state = RAMPDWN;
          state_tmr = currentMillis;
          inFlight = BURP;
        }
        break;
      }
    case BURP:
      {
        curThrottle = BurpMax;
        if (currentMillis - state_tmr > BURPTIME)
        {
          curThrottle = 180;
          state_tmr = 0;
          inFlight = RDYLAND;
        }
        break;
      }
    case RDYLAND:
      {
        if ((currentMillis - state_tmr) > RDYTIME)
        {
          state_tmr = 0;
          fly_state = RAMPDWN;
          inFlight = FLYING;
        }
      }
  }
}



void speedState()
{
  currentMillis = millis();
  switch (fly_state)
  {
    case WAIT:
      {
        led3.on();
        led4.off();
        led5.off();
        if (/*Sw1.isPressed()*/ digitalRead(SW1)== 0)  // fix this later bitton method doesnt clear latch
         {
          state_tmr = currentMillis;
          curThrottle = 0;
          fly_state = ARMED;

        }
        break;
      }
     
    case ARMED:
      {
        if ( (currentMillis - state_tmr) > cheze.ArmTime*1000)
        {

          led3.off();
          led4.on();
          led5.off();
          fly_state = TAKEOFF;
          curThrottle = 0;
          state_tmr = currentMillis;
          
        }
        break;
      }
    case TAKEOFF:
      {
        led3.on();
         led4.on();
         led5.off();

        if (curThrottle  >= cheze.FlySpeed)
        {
          fly_state = FLY;
          state_tmr = currentMillis;
          curThrottle = cheze.FlySpeed ;
 

        }
        else if (currentMillis - incTime > INCTIME)
        {
          curThrottle = curThrottle + IncThrottle;
          incTime = currentMillis;
        }

        break;
      }
    case FLY:
      {
       flyState();
        led3.off();
        led4.off();
        led5.on();


        break;
      }
    case RAMPDWN:
      {
        led3.on();
        led4.off();
        led5.on();


        if ((curThrottle) <= 10 )
        {
          fly_state = LAND;
          curThrottle = 0;
          incTime = currentMillis;
          state_tmr = currentMillis;
        }
        else if (currentMillis - incTime > INCTIME)
        {
          curThrottle = curThrottle - IncThrottle;
          incTime = currentMillis;
        }
        break;
      }
    case LAND:
    default:
      {
        led3.off();
        led4.on();
        led5.on();

        if ( currentMillis - state_tmr > LANDTIME)
        {
          fly_state = WAIT;
          state_tmr =currentMillis;
        }
       
        curThrottle = 0;
        break;
      }
  }
  if(fly_state != LAND){
  esc.write(curThrottle);
  }
}
