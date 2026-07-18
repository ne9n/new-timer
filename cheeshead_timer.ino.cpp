# 1 "C:\\Users\\310323\\AppData\\Local\\Temp\\tmpbjclpl2i"
#include <Arduino.h>
# 1 "C:/Users/310323/Desktop/cheeshead_timer/cheeshead_timer.ino"

#include <EEPROM.h>
#include <Servo.h>


#include "gyro.h"
#include "term.h"
#include "led.h"
#include "cheesehead_timer.h"
#include "ButMenu.h"
#include "state_machine.h"
# 24 "C:/Users/310323/Desktop/cheeshead_timer/cheeshead_timer.ino"
bool run_state;
int sindex;
int gyroEn;
param TimerSetup;
speed_state ispeed_state = WAIT;

Servo esc;
extern unsigned long state_timer[];


int maxThrottle = TimerSetup.FlySpeed[0];





int incTime = 0;
void setup();
void getDips();
void loop();
#line 44 "C:/Users/310323/Desktop/cheeshead_timer/cheeshead_timer.ino"
void setup()
{
  Serial.begin(19200);
  Serial.print(" init start ");


  esc.attach(SERVO, 1000, 2000);
  esc.write(0);

  int eeAddress = 0;
  bool firstRun = false;
  EEPROM.get(eeAddress, TimerSetup );
  if (TimerSetup.FlySpeed[2] > 500)
  {

      TimerSetup.calX= 1;
      TimerSetup.calY = 1;
      TimerSetup.calZ= 1;
      TimerSetup.px= 1;
      TimerSetup.py= 1;
      TimerSetup.rx= 1;
      TimerSetup.ry= 1;
      TimerSetup.PitchExThresh = 40;
      TimerSetup.YawRateExThresh = 20;
      TimerSetup.LapCount = 0;
      TimerSetup.LapLimit = 0;
      TimerSetup.FlySpeed[0] = 120; TimerSetup.FlySpeed[1]= 140; TimerSetup.FlySpeed[2]= 180;
      TimerSetup.FlyTime[0] = 40000 ;TimerSetup.FlyTime[1]= 40000 ; TimerSetup.FlyTime[2] = 40000;
      TimerSetup.ArmTime[0] = 500;TimerSetup.ArmTime[1]= 500;TimerSetup.ArmTime[2] = 500;
      TimerSetup.accelTime[0] = 5000;TimerSetup.accelTime[1] = 5000;TimerSetup.accelTime[2] = 5000;
      TimerSetup.autoSpeedPerMin = 2;
      TimerSetup.axisPitch = 0;
      TimerSetup.axisRoll = 1;
      TimerSetup.axisYaw = 2;
      EEPROM.put(eeAddress, TimerSetup);
      Serial.print(" set default values \n");
      firstRun = true;
  }
  state_timer[int (speed_state::FLY)] = TimerSetup.FlyTime[0];
  state_timer[int (speed_state::ARMED)] = TimerSetup.ArmTime[0];
  state_timer[int (speed_state::TAKEOFF_RAMP)] = TimerSetup.accelTime[0];


  ButtonSetup();

  pinMode(BUTTONPIN, INPUT_PULLUP);


  pinMode(DS1, INPUT_PULLUP);
  pinMode(DS2, INPUT_PULLUP);
  pinMode(DS3, INPUT_PULLUP);


  pinMode(LED_BUILTIN, OUTPUT);



  Wire.begin();
  mpu_setup();
  if (firstRun)
  {
    Serial.println(F("First run detected - running MPU calibration"));
    setUpMPU();
  }

  getDips();

  initLED();
  Serial.print(" init complte \n");
  Serial.println(F("Ready. Press '?' for menu, '!' to toggle telemetry (19200 baud)."));

}

void getDips()
{


  sindex = (!digitalRead(DS2)<< 1) + (!digitalRead(DS1)) ;
  gyroEn = digitalRead(DS3);
# 134 "C:/Users/310323/Desktop/cheeshead_timer/cheeshead_timer.ino"
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
  telemetryUpdate();
  if (run_state)
  {
    check_state();
    speedState();
  }
  term_ctrl();
}