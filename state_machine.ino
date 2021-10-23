
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
  char m1;

  currentMillis = millis();
  switch (fly_state)
  {
    case WAIT:
      {
          digitalWrite(LED3,HIGH);
          digitalWrite(LED4,LOW);
          digitalWrite(LED5,digitalRead(BUTTONPIN));
          m1 =Serial.read();
        if (m1 == '?')
        {
           terminal(); 
        }
        // need to see a low to high transistion
        if (!digitalRead(BUTTONPIN))
        {
          state_tmr = currentMillis;
          curThrottle = 0;
          fly_state = ARMED;
          digitalWrite(LED3,HIGH);
          digitalWrite(LED4,HIGH);
          digitalWrite(LED5, HIGH);
        }
        break;
      }
    case ARMED:
      {
        if ( (currentMillis - state_tmr) > cheze.ArmTime*1000)
        {
          digitalWrite(LED3,LOW);
          digitalWrite(LED4,HIGH);
          digitalWrite(LED5,LOW);

          fly_state = TAKEOFF;
          curThrottle = 0;
          state_tmr = currentMillis;
          
        }
        break;
      }
    case TAKEOFF:
      {
        if (curThrottle  >= cheze.FlySpeed)
        {
          fly_state = FLY;
          state_tmr = currentMillis;
          curThrottle = cheze.FlySpeed ;
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
