/*
 *  added comments
 *  
 */

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
        lights[0].skip = 1;
        lights[0].enable = HIGH;
        lights[1].skip = 4;
        lights[1].enable = HIGH;

       
        if (!digitalRead(BUTTONPIN))
        {
          state_tmr = currentMillis;
          curThrottle = 0;
          fly_state = ARMED;
        
        break;
      }
     }
    case ARMED:
      {
        if ( (currentMillis - state_tmr) > cheze.ArmTime*1000)
        {

        lights[0].skip = 2;
        lights[0].enable = HIGH;
        lights[1].skip = 1;
        lights[1].enable = HIGH;


          fly_state = TAKEOFF;
          curThrottle = 0;
          state_tmr = currentMillis;
          
        }
        break;
      }
    case TAKEOFF:
      {
        lights[0].skip = 2;
        lights[0].enable = HIGH;
        lights[1].skip = 2;
        lights[1].enable = HIGH;

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
        lights[0].skip = 2;
        lights[0].enable = HIGH;
        lights[1].skip = 1;
        lights[1].enable = LOW;


        break;
      }
    case RAMPDWN:
      {
        lights[0].skip = 2;
        lights[0].enable = LOW;
        lights[1].skip = 1;
        lights[1].enable = HIGH;


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
        lights[0].skip = 4;
        lights[0].enable = HIGH;
        lights[1].skip = 4;
        lights[1].enable = HIGH;

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
