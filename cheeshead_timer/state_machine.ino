/*
 *  
 *  speedState  manages idle, ramp up and landing phases.   
 *  could be combined into a single state machine.   
 *  state variable is speed_state
 */




void speedState()
{
  switch (speed_state)
  {
    case WAIT:
    {
          // once second burst
          curThrottle = 25; /* a quick burp */
    }
        break;
    case ARMED:
    {
          curThrottle = 0;
 
    }
    break;
    case TAKEOFF_RAMP:
    {
        delay(25);
        curThrottle++;
        if(curThrottle > MAX_SPEED )
        {
          curThrottle= MAX_SPEED;
        }
  
        
    }
    break;
    case TAKEOFF:
    {
        curThrottle = MAX_SPEED;
    }
    break;
    case FLY:
    {
        curThrottle = TimerSetup.FlySpeed;
    }
    break;
    case BURP:
    {
        curThrottle = BurpMax;
  
    }
    break;

    case RDYLAND:
    {
        curThrottle = TimerSetup.FlySpeed;
    }
    break;
    case RAMPDWN:
    {
        curThrottle = 0;
        go_fly = false;
        speed_state = WAIT;   
    }
    break;
    default:
    {
       
        curThrottle = 0;
        go_fly = false;
        speed_state = WAIT;
    }
    break;
  }
  esc.write(curThrottle);

}
