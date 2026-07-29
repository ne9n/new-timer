#include "cheesehead_timer.h"
#include "led.h"
#include <Arduino.h>

extern speed_state ispeed_state;
extern bool run_state;
extern int motorThrottle;

ezLED rled(LED5);
ezLED yled(LED4);
ezLED gled(LED3);


void initLED()
{
  gled.blinkNumberOfTimes(5000,  1000, 10);
  delay(1000);
  yled.blinkNumberOfTimes(5000, 1000, 10);
  
  delay(1000);
  yled.blinkNumberOfTimes( 5000, 1000, 10);


  rled.blink(500,500);
  Serial.print("LED init \n");

  
}

void updateLED()
{
  rled.loop();
  yled.loop();

  // Option A: Dynamic Green LED (LED3 / gled) flashing proportional to motor speed
  if (run_state && motorThrottle > 0 && 
     (ispeed_state == TAKEOFF_RAMP || ispeed_state == TAKEOFF || ispeed_state == FLY || ispeed_state == BURP))
  {
    static unsigned long lastGledToggle = 0;
    static bool gledState = false;
    // Map motorThrottle (1..MAX_SPEED/180) to blink interval (500ms down to 25ms)
    unsigned long interval = map(constrain(motorThrottle, 1, MAX_SPEED), 1, MAX_SPEED, 500, 25);
    
    if (millis() - lastGledToggle >= interval) {
      lastGledToggle = millis();
      gledState = !gledState;
      digitalWrite(LED3, gledState ? HIGH : LOW);
    }
  }
  else
  {
    gled.loop(); // Standard ezLED behavior for idle/arming/other states
  }
}


void set_led(int is)
{
  static int lastState = -1;
  if (is == lastState) return; // Prevent continuous re-initialization inside loop()
  lastState = is;

  switch (is)
  {
    case WAIT:
    {
      rled.blink(250, 1000);
      yled.turnOFF();
      gled.turnOFF();
    }
    break;
    case ARMED:
    {
      rled.turnOFF();
      yled.blink(250, 1000);
      gled.turnOFF();
    }
    break;
    case TAKEOFF_RAMP:
    {
      rled.turnOFF();
      yled.turnON();
      // Green LED updated dynamically in updateLED()
    }
    break;
    case TAKEOFF:
    {
      rled.turnOFF();
      yled.turnOFF();
      // Green LED updated dynamically in updateLED()
    }
    break;
    case FLY:
    {
      rled.turnOFF();
      yled.blink(500, 500);
      // Green LED updated dynamically in updateLED()
    }
    break;
    case BURP:
    {
      rled.turnON();
      yled.turnON();
      // Green LED updated dynamically in updateLED()
    }
    break;
    case RDYLAND:
    case RAMPDWN:
    {
      rled.blink(250, 1000);
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
  return; 
}
