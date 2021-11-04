/*
    added comments

*/

#include "led.h"
#include <Ramp.h>
#include <NoDelay.h>

ramp accelramp;
noDelay StateTime(5000);//Creats a noDelay variable




void speedState()
{
  tTime = StateTime.update();
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
        if (/*Sw1.isPressed()*/ digitalRead(SW1) == 0) // fix this later bitton method doesnt clear latch
        {

          curThrottle = 0;
          fly_state = ARMED;
          StateTime.setdelay(cheze.ArmTime * 1000);



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
          accelramp.go(curThrottle);
          accelramp.go(cheze.FlySpeed, cheze.accelTime);

        }
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
        curThrottle = accelramp.update();
        if (accelramp.isFinished())
        {
          fly_state = FLY;
          StateTime.setdelay(cheze.FlyTime * 1000);
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
        led4.off();
        led5.flash();
        curThrottle = cheze.FlySpeed + posTrim;
        if ( StateTime.update())
        {
          accelramp.go(curThrottle);
          accelramp.go(0, cheze.accelTime);
        }


        break;
      }
    case RAMPDWN:
      {
        led3.flash();
        led4.off();
        led5.flash();

        curThrottle = accelramp.update();
        if (accelramp.isFinished())
        {
          fly_state = WAIT;
          curThrottle = 0;
        }
        break;
      }
  }
  esc.write(curThrottle);
}
