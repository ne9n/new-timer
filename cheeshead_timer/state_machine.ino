
void flyState()
{
  unsigned long currentMillis1 = millis();

  switch (inFlight)
  {
    case FLYING:
      {
        if ( (currentMillis1 - state_tmr) > cheze.FlyTime)
        {
          //fly_state = RAMPDWN;
          state_tmr = millis();
          inFlight = BURP;
        }
        break;
      }
    case BURP:
      {
        curThrottle = BurpMax;
        if (currentMillis1 - state_tmr > BURPTIME)
        {
          curThrottle = 180;
          state_tmr = 0;
          inFlight = RDYLAND;
        }
        break;
      }
    case RDYLAND:
      {
        if ((currentMillis1 - state_tmr) > RDYTIME)
        {
          state_tmr = 0;
          fly_state = RAMPDWN;
        }
      }
  }
}


void speedState()
{
  unsigned long currentMillis = millis();

  switch (fly_state)
  {
    case WAIT:
      {
        // need to see a low to high transistion
        if (digitalRead(buttonPin))
        {
          state_tmr = millis();
          curThrottle = 0;
          fly_state = ARMED;
        }
        else
        {

        }
        break;
      }
    case ARMED:
      {
        if ( currentMillis - state_tmr > cheze.ArmTime)
        {
          fly_state = TAKEOFF;
          curThrottle = 0;
          incTime = currentMillis;
          state_tmr = millis();
        }
        break;
      }
    case TAKEOFF:
      {
        if (curThrottle  >= cheze.FlySpeed)
        {
          fly_state = FLY;
          state_tmr = millis();
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
        incTime = currentMillis;
        break;
      }
    case RAMPDWN:
      {

        if ((curThrottle) <= 10 )
        {
          fly_state = LAND;
          curThrottle = 0;
          incTime = currentMillis;
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
        curThrottle = 0;
        break;
      }
  }
  if (fly_state < LAND)
  {
  }


  esc.write(curThrottle);
}
