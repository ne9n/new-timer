/*
 *  
 *  speedState  manages idle, ramp up and landing phases.   
 *  could be combined into a single state machine.   
 *  state variable is speed_state
 */

/*

*/
#include <Arduino.h>
#include <Servo.h>
#include "state_machine.h"
#include "cheesehead_timer.h"

extern param TimerSetup;
extern Servo esc;
extern bool run_state;
extern speed_state ispeed_state;
extern int iangleZ;
extern void set_led(int);
unsigned long currentMillis;
unsigned long previousMillis;
int lastZ = 0;
long angle_time = 0;
int curThrottle;
bool gyro_flag = false;

extern bool PitchEX;
extern bool YawEX;
extern bool YawLOW;


 /*defaultvalues for states up above */
 unsigned long state_timer[] = {

  1000, /* 0 wait*/
  3000, /* 1 armed*/
  5000, /*2  spin up */
  5000, /* 3 full speed */
  600000, /*4 fly*/
  500, /*5 Burp*/
  2000, /*6 ready land */
  3000  /* 7 land*/
 };




bool check_state()
{
  currentMillis = millis();
 /* delay(1000);
  Serial.print(ispeed_state); 
  Serial.print("  ");
  Serial.print((currentMillis - previousMillis));
  Serial.print("  ");
  Serial.print (state_timer[ispeed_state]);
  Serial.print(" check state info \n"); 
*/
  if  ((currentMillis - previousMillis)  > (state_timer[ispeed_state]))
  {
  
  
    
    // inc the state and make it wrap 8 states 

   // advance state
   ispeed_state = static_cast <speed_state>((static_cast<int>((ispeed_state)+1)%8));
    previousMillis = currentMillis;
    
    return true;
  
   }
   else
    return false;
}

// Set the current speed state and reset the internal previousMillis
void setSpeedState(speed_state newState)
{
  ispeed_state = newState;
  previousMillis = millis();
}

void speedState()
{


 /* Serial.print(ispeed_state); 
  Serial.print(" =state ");
  Serial.print(( curThrottle));
  Serial.print(" =thottle  ");
  Serial.print(( run_state));
  Serial.print(" =run state   ");
  
  Serial.print(" SpeedState \n"); 

  delay(1000);*/

  set_led(ispeed_state);
  switch (ispeed_state)
  {
    case speed_state:: WAIT:
    {
      // once second burst
      curThrottle = 25; /* a quick burp */
      PitchEX = false;
      YawEX =false;
      YawLOW = false;

      // Serial.print("wait   ");
    }
    break;
    case speed_state::ARMED:
    {
      curThrottle = 0;

    }
    break;
    case speed_state::TAKEOFF_RAMP:
    {
      //
      delay(25);
      angle_time = currentMillis; //pre load this for crashcheck
      lastZ = iangleZ - 10;
      curThrottle++;
      if (curThrottle > MAX_SPEED)
      {
        curThrottle = MAX_SPEED;
      }
    }
    break;
    case speed_state::TAKEOFF:
    {
      curThrottle = MAX_SPEED;
      // init values for crash_check
      if (PitchEX or YawEX)
      {
        curThrottle = 0;
        run_state =false;
      }
    }
    break;
    case speed_state::FLY:
    {
      curThrottle = TimerSetup.FlySpeed[0];
      if (PitchEX or YawEX or YawLOW)
      {
        curThrottle = 0;
        run_state = false;
      }
    }
    break;
    case speed_state::BURP:
    {
      curThrottle = BurpMax;
    }
    break;
    case speed_state::RDYLAND:
    {
      curThrottle = TimerSetup.FlySpeed[0];
      
    }
    break;
    case speed_state::RAMPDWN:
    {
      curThrottle = 0;
      run_state = false; 
  
    }
    break;
    default:
    {
      curThrottle = 0;
      run_state = false;
 
    }
    break;
  }
  // apply pitch trim (`posTrim`) and maneuver boost (`maneuverBoost`) and clamp throttle
  extern int posTrim;
  extern int maneuverBoost;
  long adjusted = (long)curThrottle + (long)posTrim + (long)maneuverBoost;
  if (adjusted < 0) adjusted = 0;
  if (adjusted > MAX_SPEED) adjusted = MAX_SPEED;
  esc.write((int)adjusted);
}
