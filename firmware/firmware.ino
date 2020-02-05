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
#define ARMED 3
#define FLY 4
#define RAMPDWN 5
#define LAND 6

Servo esc;

// definations 
int maxThrottle = MaxSpeed;
int state_tmr;
int curThrottle = 0; // current speed 



int fly_state = WAIT;

void setup() {
	// attaches the servo on pin 9 to the servo object
	esc.attach(9, 1000, 2000);
	esc.write(curThrottle);
  pinMode(7, INPUT);  
}


void loop() 
{
 unsigned long currentMillis = millis();
 
 switch (fly_state)
 {
    case WAIT:
    {
      x - debounce
      if (digitalRead(7))
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
    {
      curThrottle = 0;
      break;
    }
 }
 esc.write(curThrottle);
}
