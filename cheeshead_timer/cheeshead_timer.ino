#include <Servo.h>
#include <EEPROM.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <ezLED.h>
#include "ADebouncer.h"



// Dave Siegler ne9n.dave@gmail.com
// This is a timer that steps through a seguence of speeds
// to control the throttle poistion of a control line airplane
// in includes user set up values
// LED;s sho state of the time
// an MUP is used to trim the throttle based on
// pitch and roll values of te airplane
// create servo object to control a servo
// state constants part of config values
//

// acclermotoer support using MPU6050 tockn laibrat

// SHORT TERM
// 1) improve LED ,
// 2) clean up menu
// 3) range checl control functions
// 4) better burp, on startup 
// 5) Draw it up and write docs

// LONG TERM
// 1) RC transmitter interface
// 2) Add acceleration boost to control function

typedef struct {
  unsigned int FlySpeed; // 0 to 180 for degrees of rotation of a servo
  unsigned int FlyTime; // air time in seconds
  unsigned int ArmTime; // wait time in seconds
  unsigned int accelTime; // in msec
  int calX;
  int calY;
  int calZ;
  byte px;
  byte py;
  byte rx;
  byte ry;
} param;

//int i;
param TimerSetup;
MPU6050 mpu6050(Wire);
Servo esc;
ADebouncer debouncer; 

#define MAX_SPEED  180
#define IncThrottle 1
#define BurpMax 180



enum  speed_s
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
 enum speed_s speed_state = WAIT;
 /*defaultvalues for states up above */
 unsigned long state_timer[] = {
  1000, /* 0 wait*/
  3000, /* 1 armed*/
  5000, /*2  spin up */
  5000, /* 3 full speed */
  20000, /*4 fly*/
  1000, /*5 Burp*/
  5000, /*6 ready land */
  1500  /* 7 land*/
 };





// hardware pin def's
#define BUTTONPIN 10
#define SW1 8
#define SERVO 3
#define LED3 7
#define LED4 8
#define LED5 9 //works





extern void terminal();
extern void speedState();
extern void gyro();
extern void printDebug();




// Gyro related
bool gyro_flag = false;
bool go_fly = false;

int angleX;
int angleY;
int angleZ;

// speed variables
int maxThrottle = TimerSetup.FlySpeed;
int curThrottle = 0; // current speed
int posTrim;


// time variables 
unsigned long currentMillis = millis();
unsigned long previousMillis;
int incTime = 0;
  ezLED rled(LED5);
  ezLED yled(LED4);
  ezLED gled(LED3);

// state variables





void setup()
{
  Serial.begin(19200);

  Serial.print(" init start");
  
  // attaches the servo on pin 9 to the servo object
  esc.attach(SERVO, 1000, 2000);
  esc.write(curThrottle);
  // read in all the set up variales 
  int eeAddress = 0;
  EEPROM.get(eeAddress, TimerSetup );
  state_timer[FLY] = TimerSetup.FlyTime; 
  state_timer[ARMED] = TimerSetup.ArmTime; 
  state_timer[TAKEOFF_RAMP] = TimerSetup.accelTime; 
  //mpu6050.setGyroOffsets(calX, calY, calZ);
  // set up the I/O pins
      
  pinMode(BUTTONPIN, INPUT_PULLUP);
  debouncer.mode(DELAYED, 100 , HIGH);
  pinMode(LED_BUILTIN, OUTPUT);

  Wire.begin();
  mpu6050.begin();
  //mpu6050.calcGyroOffsets(true);
  mpu6050.setGyroOffsets(TimerSetup.calX / 100.0, TimerSetup.calY / 100.0, TimerSetup.calZ / 100.0);
  rled.blink(500,500);
  Serial.print(" init complte");
 

}


void plotDebug(void)
{
   
  
  //Serial.print( (digitalRead(BUTTONPIN))  ? "\t button up": "\t button down" );
 
  //Serial.print(F("angleX:"));      Serial.print(angleX); Serial.print(F(","));
  //Serial.print(F("angleY:"));    Serial.print(angleY );Serial.print(F(","));
  //Serial.print(F("angleZ:"));    Serial.print(angleZ );Serial.print(F(","));
  //Serial.print(F("\angleZ:"));    Serial.print(angleZ );Serial.print(F(","));
  //Serial.print(F("!button:"));    Serial.print(!digitalRead(BUTTONPIN) ); Serial.print(F(","));
  //Serial.print(F("posTrim:"));  Serial.print(posTrim);Serial.print(F(","));
  //Serial.print(F("throttle:")); Serial.print(curThrottle);Serial.print(F(","));
  Serial.print(F(" speed_st:"));   Serial.print(speed_state);Serial.print(F(","));
  Serial.print(F(" go_fly:"));   Serial.print(go_fly);Serial.print(F(","));
 // Serial.print(F("currentMillis -state_tmr:")); Serial.print(currentMillis-state_tmr);
  Serial.print(F("time left: ")); Serial.print(state_timer[speed_state] - (currentMillis - previousMillis) );
  Serial.print(F("\n"));
   
  //Serial.println();
}
void term_ctrl()
{
  char keypress;
  keypress = Serial.read();
  if (keypress == '?')
  {
    terminal();
  }
}

bool check_state()
{
  currentMillis = millis();
  if  (currentMillis - previousMillis  > state_timer[speed_state])
  {
    //previousMillis = currentMillis;
    /* inc the state and make it wrap 8 states */
    speed_state = static_cast <speed_s>((speed_state+1)%8);
    previousMillis = currentMillis;
    set_led();
    return true;
  
   }
   else
    return false;
}

int i;

void loop()
{
  rled.loop();
  yled.loop();
  gled.loop();
       
  // need to see a low to high transistion

  bool buttonState = debouncer.debounce(digitalRead(BUTTONPIN));
  bool falling = debouncer.falling();
  if (falling)
  {
    
    if (go_fly)
    {
       previousMillis = currentMillis;
       speed_state = WAIT;
       set_led();
       curThrottle = 0;  
       go_fly = false;
       Serial.println("dont");
    }
    else
    { 
      go_fly = true;
      previousMillis = currentMillis;
      speed_state = WAIT;
      curThrottle = 0;  
      Serial.println("do");

    }
  previousMillis = currentMillis;
  speed_state = WAIT;

  
  }
  term_ctrl();
  if (go_fly)
  {
    speedState();
    if (check_state())
    {
       // this manages the state machunes for takeoff flyingand landing
      set_led();
      //Serial.print(F("state:")); Serial.println(speed_state);
      //Serial.println(F(" timer:")); Serial.println (state_timer[speed_state]);
     // Serial.print(F("throttle :"));Serial.println(curThrottle );
      
      
    
    }
  }

  if (i++ >500)
  {
    plotDebug();  // this prints a serial stream for the Arduino debugger.  un comment the intems to plot. 
     i = 0;
  }
  //speedGyro();       // this reads the gyro and trims the speed 

}
