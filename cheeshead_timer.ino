
#include <EEPROM.h>
#include <Servo.h>


#include "gyro.h"
#include "term.h"
#include "led.h"
#include "cheesehead_timer.h"
#include "ButMenu.h"
#include "state_machine.h"

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

// TODOs addressed: lap-limit shutdown, menu entry, and initialization
// Add dip swithc support 
// check LED colors 
// Add extra fileds to paramater structure 
// Add servo 2 support (not requred) 
// excessive movement funcgions 

// to do testsing 
// new button lib wotks 
// SS runs 




bool run_state;
int sindex;
int gyroEn;
param TimerSetup;
speed_state ispeed_state = WAIT;

Servo esc;
extern unsigned long state_timer[];

// speed variables
int maxThrottle = TimerSetup.FlySpeed[0];



// time variables 

int incTime = 0;



void setup()
{
  Serial.begin(19200);
  Serial.print(" init start ");
  // SERVO SETUP attaches the servo on pin 9 to the servo object
  esc.attach(SERVO, 1000, 2000);
  esc.write(0);
  // EEprom read in all the set up variales 
  int eeAddress = 0;
  EEPROM.get(eeAddress, TimerSetup );
  if (TimerSetup.FlySpeed[2] > 500)
  {
    /* must br a new board set defaukts */
    
      TimerSetup.calX= 1;
      TimerSetup.calY = 1;
      TimerSetup.calZ= 1;
      TimerSetup.px= 1;
      TimerSetup.py= 1;
      TimerSetup.rx= 1;
      TimerSetup.ry= 1; 
      TimerSetup.PitchExThresh = 40; // degrees per sample (legacy behavior)
      TimerSetup.YawRateExThresh = 20; // deg/sec
      TimerSetup.LapCount = 0;
      TimerSetup.LapLimit = 0; // 0 disables auto-shutdown by laps
      TimerSetup.FlySpeed[0] = 120; TimerSetup.FlySpeed[1]= 140; TimerSetup.FlySpeed[2]= 180;// 0 to 180 for degrees of rotation of a servo
      TimerSetup.FlyTime[0] = 40000 ;TimerSetup.FlyTime[1]= 40000 ; TimerSetup.FlyTime[2] = 40000; // air time in seconds
      TimerSetup.ArmTime[0] = 500;TimerSetup.ArmTime[1]= 500;TimerSetup.ArmTime[2] = 500; // wait time in seconds
      TimerSetup.accelTime[0] = 5000;TimerSetup.accelTime[1] = 5000;TimerSetup.accelTime[2] = 5000; // in mse
      EEPROM.put(eeAddress, TimerSetup);
      Serial.print(" set default values \n");
      
      }
  state_timer[int (speed_state::FLY)] = TimerSetup.FlyTime[0]; 
  state_timer[int (speed_state::ARMED)] = TimerSetup.ArmTime[0]; 
  state_timer[int (speed_state::TAKEOFF_RAMP)] = TimerSetup.accelTime[0]; 

  // set up the I/O pins
  ButtonSetup(); // sets helper funcgions of button 

  pinMode(BUTTONPIN, INPUT_PULLUP);
  // dip switches are only 

  pinMode(DS1, INPUT_PULLUP);
  pinMode(DS2, INPUT_PULLUP);
  pinMode(DS3, INPUT_PULLUP);
  
  //debouncer.mode(DELAYED, 100 , HIGH);
  pinMode(LED_BUILTIN, OUTPUT);

  
  // MPU 6050 gyro  
  Wire.begin(); // ?? 
  mpu_setup(); // in gyro.cpp

  getDips(); // need to init this
  //rled.blink(500,500);
  initLED();
  Serial.print(" init complte \n");

}

void getDips()
{
  // read the dip swichs may be need a debounce
  // allow 3 speed prfiles and on swich for enable disable the gyro
  sindex = (!digitalRead(DS2)<< 1) + (!digitalRead(DS1))  ;
  gyroEn = digitalRead(DS3);
  // install board cores (commented stray text removed)
  
/*
  Serial.print(sindex); 
  Serial.print(" = speed dips  ");
  Serial.print(( gyroEn));
  Serial.print(" =gyro enbable"); 
  Serial.print(" get dips \n"); 

  delay(1000);
*/
}




void loop()
{
  if (gyroEn)
  {
    speedGyro();
  }  
  updateLED();
  getDips();
  updateButton();
  if (run_state)
  {
    check_state();
    speedState();
  }
  term_ctrl();
}

