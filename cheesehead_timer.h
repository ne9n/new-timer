#ifndef CHEESHEADTIMER_H    // Put these two lines at the top of your file.
#define CHEESHEADTIMER_H   // (Use a suitable name, usually based on the file name.)
#include <Arduino.h>
#include <ezLED.h>
// Place your main header code here.


#define MAX_SPEED  180 // needs to ptogramabble 
#define IncThrottle 1
#define BurpMax 180

// hardware pin def's blue board white board seems to have same
#define SERVO2 2  //only on white board
#define SERVO 3
#define DS1 4 //only on white define uplled up
#define DS2 5 //only on white define uplled up
#define DS3 6 //only on white define uplled up

#define LED3 7  // colors may be wrong
#define LED4 8
#define LED5 9 //works
#define BUTTONPIN 10

extern void terminal();
extern void speedState();
extern void gyro();
extern void printDebug();
extern void set_led(int);
extern void speedGyro();
extern void updateLED();
extern void initLED();
extern int posTrim; // pitch trim applied to throttle (can be negative)
extern int maneuverBoost; // positive boost applied for maneuvering (computed from pitch)

typedef struct {
 
  int calX;
  int calY;
  int calZ;
  byte px;
  byte py;
  byte rx;
  byte ry; 
  // thresholds for excessive movement detection
  int PitchExThresh;    // per-sample pitch delta threshold (degrees)
  int YawRateExThresh;  // yaw rate threshold (deg/sec)
  unsigned long LapCount; // number of tenths of laps (units = 0.1 lap)
  unsigned long LapLimit; // 0 = disabled, positive = max laps before shutdown (in tenths, 1 = 0.1 lap)
  unsigned int FlySpeed[3]; // 0 to 180 for degrees of rotation of a servo
  unsigned int FlyTime[3]; // air time in seconds
  unsigned int ArmTime[3]; // wait time in seconds
  unsigned int accelTime[3]; // in mse
  // Auto increase throttle while in flight: percent per minute (0 disables)
  byte autoSpeedPerMin; // e.g., 2 = 2% per minute
} param;

enum speed_state
{
  WAIT,
  ARMED,
  TAKEOFF_RAMP,
  TAKEOFF,
  FLY,
  BURP,
  RDYLAND,
  RAMPDWN
 };



 







#endif    // Put this line at the end of your file.