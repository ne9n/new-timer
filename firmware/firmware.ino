#include <Servo.h>
// create servo object to control a servo

// state constants part of config values 
#define MaxSpeed 120 // 0 to 180 for degrees of rotation of a servo
#define FlyTime 90000 // air time in miliseconfs
#define ArmTime 30000 // wait time in milisecnds 
#define IncThrottle 3

#define WAIT 0
#define ARMED 1
#define TAKEOFF 2
#define FLY 3
#define RAMPDWN 4
#define LAND 5

Servo esc;

// definations 
int maxThrottle = MaxSpeed;
int state_tmr;
int curThrottle = 0; // current speed 


const int buttonPin = 7;

int fly_state = WAIT;


void debounce()
{
// Variables will change:
  static int buttonState = LOW;             // the current reading from the input pin
  static int lastButtonState = LOW;   // the previous reading from the input pin

  static unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
  static unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) 
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) 
    {
      buttonState = reading;

    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  return buttonState;
}





void setup() {
	// attaches the servo on pin 9 to the servo object
	esc.attach(9, 1000, 2000);
	esc.write(curThrottle);
  pinMode(buttonPin, INPUT);  
}



void loop() 
{
 unsigned long currentMillis = millis();
 
 switch (fly_state)
 {
    case WAIT:
    {
      // need to see a low to high transistion 
      if (debounce)
      {
        state_tmr = millis();
        fly_state = ARMED;
      }
      break;
    }
    case ARMED:
    {
      if ( currentMillis- state_tmr > ArmTime)
      {
        fly_state = TAKEOFF;
        state_tmr = millis();
      }   
      break;
    }
    case TAKEOFF:
    {
      if ((curThrottle =+ IncThrottle)  >= MaxSpeed)
      {
        fly_state = FLY;
        state_tmr = millis();
        curThrottle = MaxSpeed;
      }        
      delay(100);  /* ick I hate this */
      break;
    }
    case FLY:
    {
      if ( (currentMillis - state_tmr) > FlyTime)
      {
        fly_state = RAMPDWN;
        state_tmr = millis();
      }
      break;
    }
    case RAMPDWN:
    {
      if ((curThrottle-=IncThrottle)<=0 )
      {
        fly_state = LAND;
        curThrottle = 0;  
      }
      delay(100);
      break;
    }
    case LAND:
    default:
    {
      curThrottle = 0;
      break;
    }
 }
 esc.write(curThrottle);
}
