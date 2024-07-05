
void set_led()
{
  switch (speed_state)
  {
    case WAIT:
    {
          rled.blink(250,1000);
          yled.turnOFF();
          gled.turnOFF();

    }
        break;
    case ARMED:
    {
          rled.turnOFF();
          yled.blink(250,1000);
          gled.turnOFF();
    }
    break;
    case TAKEOFF_RAMP:
    {
          rled.turnOFF();
          yled.turnON();
          gled.blink(250,100);

        
    }
    break;
    case TAKEOFF:
    {
        rled.turnOFF();
        yled.turnOFF();
        gled.turnON();
    }
    break;
    case FLY:
    {
        rled.turnOFF();
        yled.blink(500,500);
        gled.blink(500,500);
    }
    break;
    case BURP:
  {
        rled.turnON();
        yled.turnON();
        gled.turnON();

    }
    break;

    case RDYLAND:
    {

    }
    break;
    case RAMPDWN:
    {

        rled.blink(250,1000);
          yled.turnOFF();
          gled.turnOFF();
       
    }
    break;
    default:
    {
        rled.turnON();
        yled.turnOFF();
        gled.turnOFF();
       
      
    }
    break;
  }
}