#include <Servo.h>
// create servo object to control a servo
// state constants part of config values 

// LIST OF THINGS TO DO
// START BUTTON no debounce
// Need LED support 
// programming method
// acclermotoer support
// accel and decel ramps work but need a better method 
 

#define MaxSpeed 120 // 0 to 180 for degrees of rotation of a servo
#define FlyTime 90000 // air time in miliseconfs
#define ArmTime 30000 // wait time in milisecnds 
#define IncThrottle 1
#define BurpMax 180
#define BURPTIME 500
#define RDYTIME 5000
//for tesr
#define FlyTime 9000 // air time in miliseconfs
#define ArmTime 3000 // wait time in milisecnds 

#define FLASHON 500
#define FLASHOFF 500

#define WAIT 0
#define ARMED 1
#define TAKEOFF 2
#define FLY 3
#define RAMPDWN 4
#define LAND 5

// sub state for flyng
#define FLYING 0
#define BURP 1
#define RDYLAND 2



#define INCTIME 50

Servo esc;

// definations 
int maxThrottle = MaxSpeed;
int state_tmr;
int curThrottle = 0; // current speed 
int incTime = 0;
int led_state =0;

const int buttonPin = 7;

int fly_state = WAIT;
int inFlight = FLYING;


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
  return !buttonState;
}





void setup() {
	// attaches the servo on pin 9 to the servo object
	esc.attach(9, 1000, 2000);
	esc.write(curThrottle);
  pinMode(buttonPin, INPUT); 
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void ledUpdate()
{
  unsigned long currentMillis1 = millis();

  switch (led_state)
  {
    case 1:
    {
      digitalWrite(LED_BUILTIN, HIGH);
      led_state = 2;
      break;  
    }
    case 2:
    {
      digitalWrite(LED_BUILTIN, LOW);
      led_state = 1;
      break;  
     
    }
  }
}
void flyState()
{
unsigned long currentMillis1 = millis();

  switch (inFlight)
  {
    case FLYING:
    {
     if ( (currentMillis1 - state_tmr) > FlyTime)
      {
        //fly_state = RAMPDWN;
        state_tmr = millis();
        inFlight= BURP;
      }
      break;
    }
    case BURP:
    {
      curThrottle = BurpMax;
      if (currentMillis1 - state_tmr > BURPTIME)
      {
        curThrottle = MaxSpeed;
        state_tmr = 0;
        inFlight = RDYLAND;
      }
      break;
    }
    case RDYLAND:
    {
      if((currentMillis1 - state_tmr) > RDYTIME)
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
      break;
    }
    case ARMED:
    {
      if ( currentMillis- state_tmr > ArmTime)
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
      Serial.print(curThrottle, DEC);
      Serial.print("\n");
      if (curThrottle  >= MaxSpeed)
      {
        fly_state = FLY;
        state_tmr = millis();
        curThrottle = MaxSpeed;
        
      }        
      else if (currentMillis- incTime >INCTIME)
      {
        curThrottle = curThrottle+ IncThrottle;
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

      if ((curThrottle)<=10 )
      {
        fly_state = LAND;
        curThrottle = 0;  
        incTime = currentMillis;
      }
      else if (currentMillis- incTime >INCTIME)
      {
        curThrottle= curThrottle - IncThrottle; 
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
 Serial.print(curThrottle, DEC);
 Serial.print("\n");

 esc.write(curThrottle);
}
void loop()
{
  speedState();
  ledUpdate();
}
