
void flyState()
{
  unsigned long currentMillis1 = millis();

  switch (inFlight)
  {
    case FLYING:
      {
        curThrottle = cheze.FlySpeed + posTrim;
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
  //Serial.println(fly_state);
  //Serial.println(curThrottle);
 
  switch (fly_state)
  {
    case WAIT:
      {
          digitalWrite(3,LOW);
          digitalWrite(4,LOW);
          digitalWrite(5,LOW);
 
        // need to see a low to high transistion
        if (digitalRead(buttonPin))
        {
          state_tmr = millis();
          curThrottle = 0;
          fly_state = ARMED;
          digitalWrite(3,LOW);
          digitalWrite(4,LOW);
          digitalWrite(5, LOW);
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
          digitalWrite(3, LOW);
          digitalWrite(4,HIGH);
          digitalWrite(5,LOW);

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
          digitalWrite(3,HIGH);
          digitalWrite(4,HIGH);
          digitalWrite(5, LOW);

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
        digitalWrite(3, LOW);
        digitalWrite(4, LOW);
        digitalWrite(5 ,HIGH);

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
  constrain(curThrottle, 0,180);
  Serial.print("throttle : ");
  Serial.print(curThrottle);
  Serial.print("\t trim : ");
  Serial.print(posTrim);
  Serial.println();
  if(fly_state != LAND){
  esc.write(curThrottle);
  }
}
