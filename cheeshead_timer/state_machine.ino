/*
 * Flystate() state machine only maanges the flying segment of the 
 *  ptofile  state variable is inflight
 *  
 *  speedState  manages idele, ramp up and landing phases.   
 *  could be combined into a single state machine.   
 *  state variable is speed_state
 */

void flyState()
{
  switch (inFlight)
  {
    case FLYING:
      {
        curThrottle = TimerSetup.FlySpeed + posTrim;
        if ( (currentMillis- state_tmr) > TimerSetup.FlyTime*1000 )
        {
           //speed_state = RAMPDWN;
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
          speed_state = RAMPDWN;
          inFlight = FLYING;
        }
      }
  }
}


void speedState()
{
  currentMillis = millis();
  switch (speed_state)
  {
    case WAIT:
      {
          digitalWrite(LED3,HIGH);
          digitalWrite(LED4,LOW);
          digitalWrite(LED5,LOW);
        // need to see a low to high transistion
        if (!digitalRead(BUTTONPIN))
        {
          state_tmr = currentMillis;
          curThrottle = 0;
          speed_state = ARMED;
          digitalWrite(LED3,HIGH);
          digitalWrite(LED4,HIGH);
          digitalWrite(LED5, HIGH);
        
        break;
      }
     }
    case ARMED:
      {
        if ( (currentMillis - state_tmr) > TimerSetup.ArmTime*1000)
        {
          digitalWrite(LED3,LOW);
          digitalWrite(LED4,HIGH);
          digitalWrite(LED5,LOW);

          speed_state = TAKEOFF;
          curThrottle = 0;
          state_tmr = currentMillis;
          
        }
        break;
      }
    case TAKEOFF:
      {
        if (curThrottle  >= TimerSetup.FlySpeed)
        {
          speed_state = FLY;
          state_tmr = currentMillis;
          curThrottle = TimerSetup.FlySpeed ;
          digitalWrite(LED3,HIGH);
          digitalWrite(LED4,HIGH);
          digitalWrite(LED5, LOW);

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
        digitalWrite(LED3, LOW);
        digitalWrite(LED4, LOW);
        digitalWrite(LED5 ,HIGH);

        break;
      }
    case RAMPDWN:
      {

        if ((curThrottle) <= 10 )
        {
          speed_state = LAND;
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
        if ( currentMillis - state_tmr > LANDTIME)
        {
          speed_state = WAIT;
          state_tmr =currentMillis;
        }
       
        curThrottle = 0;
        break;
      }
  }
  if(speed_state != LAND){
  esc.write(curThrottle);
  }
}
