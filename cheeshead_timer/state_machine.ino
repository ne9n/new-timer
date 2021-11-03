/*
 *  added comments
 *  
 */

#include "led.h"

noDelay StateTime(5000);//Creats a noDelay variable 

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
          lastMillis = currentMillis;
          inFlight = BURP;
        }
        break;
      }
    case BURP:
      {
        curThrottle = BurpMax;
        if (currentMillis - lastMillis > BURPTIME)
        {
          curThrottle = 180;
          lastMillis = currentMillis;
          inFlight = RDYLAND;
        }
        break;
      }
    case RDYLAND:
      {
        if ((currentMillis - lastMillis) > RDYTIME)
        {
          lastMillis = currentMillis;
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
    /*
    Waits here until the start button is pressed
    Entrance -  Start up or land
    Exit Armed state 
    */
      {
        led3.flash();
        led4.flash();
        led5.flash();
        if (/*Sw1.isPressed()*/ digitalRead(SW1)== 0)  // fix this later bitton method doesnt clear latch
        {
          
          curThrottle = 0;
          fly_state = ARMED;
          StateTime.setdelay(cheze.ArmTime*1000);
          

        }
        break;
      }
     
    case ARMED:
      {     
        /* waits here to walk out to the model
          Entry from wait state 
          Exit timer expired and on to take off */     
        led3.off();
        led4.flash();
        led5.off();

        if ( StateTime.update())
        {

          fly_state = TAKEOFF;
          curThrottle = 0;
          incThrottle = 
          
          
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
       
         led3.flash();
         led4.flash();
         led5.off();

        if (curThrottle  >= cheze.FlySpeed)
        {
          fly_state = FLY;
          lastMillis = currentMillis;
          curThrottle = cheze.FlySpeed ;
 

        }
        else if (currentMillis - lastMillis > INCTIME)
        {
          curThrottle = curThrottle + INCTHROTTLE;
          lastMillis = currentMillis;
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
        flyState();
        led3.off();
        led4.off();
        led5.flash();


        break;
      }
    case RAMPDWN:
      {
        led3.flash();
        led4.off();
        led5.flash();


        if ((curThrottle) <= 10 )
        {
          fly_state = LAND;
          curThrottle = 0;
          lastMillis = currentMillis;
         }
        else if (currentMillis - lastMillis > INCTIME)
        {
          curThrottle = curThrottle - INCTHROTTLE;
          lastMillis = currentMillis;
        }
        break;
      }
    case LAND:
    default:
      {
        led3.on();
        led4.on();
        led5.on();

        if ( currentMillis - lastMillis > LANDTIME)
        {
          fly_state = WAIT;
          lastMillis =currentMillis;
        }
       
        curThrottle = 0;
        break;
      }
  }
  if(fly_state != LAND){
  esc.write(curThrottle);
  }
}
