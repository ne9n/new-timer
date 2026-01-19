
#include <EEPROM.h>
#include <MPU6050_tockn.h> 
//#include <hardwareSerial.h>
#include "term.h"
#include "cheesehead_timer.h"

extern void setUpMPU(void);
extern param TimerSetup;
extern unsigned long state_timer[];
void terminal();

void banner()
{
  Serial.flush();
  Serial.println (F("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"));
  Serial.println (F(" Cheesehead Timer"));
  Serial.println (F(" A control line time and speed regulator"));
  Serial.println (F(" https://github.com/ne9n"));
  Serial.println (F(" Dave Siegler ne9n.dave@gmail.com"));
  Serial.println (F("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"));

}

/*
void plotDebug(void)
{
   
  
  //Serial.print( (digitalRead(BUTTONPIN))  ? "\t button up": "\t button down" );
 
  Serial.print(F("angleX:"));      Serial.print(angleX); Serial.print(F(","));
  Serial.print(F("angleY:"));    Serial.print(angleY );Serial.print(F(","));
  Serial.print(F("angleZ:"));    Serial.print(angleZ );Serial.print(F(","));
//  Serial.print(F("angleZ:"));    Serial.print(angleZ );Serial.print(F(","));
  //Serial.print(F("!button:"));    Serial.print(!digitalRead(BUTTONPIN) ); Serial.print(F(","));
  //Serial.print(F("posTrim:"));  Serial.print(posTrim);Serial.print(F(","));
  // Serial.print(F("throttle:")); Serial.print(curThrottle);Serial.print(F(","));
  Serial.print(F(" speed_st:"));   Serial.print(speed_state);Serial.print(F(","));
  //Serial.print(F(" go_fly:"));   Serial.print(go_fly);Serial.print(F(","));
  Serial.print(F("time left: ")); Serial.print(state_timer[speed_state] - (currentMillis - previousMillis) );
  Serial.print(F("\n"));
   
  //Serial.println();
}
*/
void term_ctrl()
{
  char keypress;
  keypress = Serial.read();
  if (keypress == '?')
  {
    terminal();
  }
  else if (keypress == '!')
  {
    // toggle telemetry mode (non-blocking)
    extern bool telemetryMode;
    telemetryMode = !telemetryMode;
    if (telemetryMode) Serial.println(F("! Telemetry ON")); else Serial.println(F("! Telemetry OFF"));
  }
  else if (keypress == 'q' || keypress == 'Q')
  {
    extern bool telemetryMode;
    if (telemetryMode)
    {
      telemetryMode = false;
      Serial.println(F("! Telemetry OFF"));
    }
  }
}
// Non-blocking telemetry printing; call frequently from main loop
bool telemetryMode = false;
void telemetryUpdate()
{
  if (!telemetryMode) return;

  extern int curThrottle;
  extern int iangleX;
  extern int iangleY;
  extern int iangleZ;
  extern int maneuverBoost;
  extern speed_state ispeed_state;

  // consume all pending incoming bytes and check for 'q' or 'Q' to exit
  while (Serial.available() > 0)
  {
    int v = Serial.read();
    if (v == 'q' || v == 'Q')
    {
      telemetryMode = false;
      Serial.println(F("! Telemetry OFF"));
      return;
    }
    // ignore other bytes (including CR/LF)
  }

  int speed = curThrottle;
  int pitch = iangleX;
  int roll = iangleY;
  int yaw = iangleZ;
  int boost = maneuverBoost;
  int led3 = digitalRead(LED3);
  int led4 = digitalRead(LED4);
  int led5 = digitalRead(LED5);
  int btn = !digitalRead(BUTTONPIN); // active low -> pressed = 1

  Serial.print(F("! speed:")); Serial.print(speed);
  Serial.print(F(" pitch:")); Serial.print(pitch);
  Serial.print(F(" roll:")); Serial.print(roll);
  Serial.print(F(" yaw:")); Serial.print(yaw);
  Serial.print(F(" boost:")); Serial.print(boost);
  Serial.print(F(" leds:R")); Serial.print(led5);
  Serial.print(F(" Y")); Serial.print(led4);
  Serial.print(F(" G")); Serial.print(led3);
  Serial.print(F(" btn:")); Serial.print(btn);
  Serial.print(F(" state:")); Serial.println((int)ispeed_state);
}


void saveData()
{

  int eeAddress = 0;   
  EEPROM.put(eeAddress, TimerSetup );
}
void getInput()
{
  char menu = ' ';
  uint8_t prof = 0;
  unsigned int val = 0;
  while (menu != 'q' )
  {
    while (Serial.available() > 0)
    {
      Serial.read();  // clear the buffer}
    }
    while ((Serial.available() == 0))
    {
    } //wait a for key
    menu = Serial.read();             // get slection
    // parse inputs per-command below (allows decimal for lap limit)
        
    switch (menu)
    {
      case 'a':
      case 'A':
        {
          prof = Serial.parseInt();
          val = Serial.parseInt();
          prof --;
          Serial.print(F( "Set fly speed "));
          TimerSetup.FlySpeed[prof] = val;
          if (TimerSetup.FlySpeed[prof] > MAX_SPEED) 
          {
            TimerSetup.FlySpeed[prof] = MAX_SPEED;
          }
          Serial.println(TimerSetup.FlySpeed[prof]);
          break;
      case 'b':
      case 'B':
        {
          prof = Serial.parseInt();
          val = Serial.parseInt();
          Serial.print(F( "Fly Time  secs is "));
          TimerSetup.FlyTime[prof] = val;
          state_timer[int (speed_state::FLY)] = TimerSetup.FlyTime[prof]; 
          Serial.println(TimerSetup.FlyTime[prof] );
          break;
        }
      case 'c':
      case 'C':
        {
          prof = Serial.parseInt();
          val = Serial.parseInt();
          Serial.print(F( "arm time in seconds is "));
          TimerSetup.ArmTime[prof] = val ;
          state_timer[int (speed_state::ARMED)] = TimerSetup.ArmTime[prof]; 
          Serial.println(TimerSetup.ArmTime[0] );
          break;
        }
      case 'D':
      case 'd':
        {
          prof = Serial.parseInt();
          val = Serial.parseInt();
          Serial.print(F( "Accel time in mseconds "));
          TimerSetup.accelTime[prof] = val ;
          state_timer[int (speed_state::TAKEOFF_RAMP)] = TimerSetup.accelTime[0]; 
          Serial.println(TimerSetup.accelTime[prof] );
          break;
        }
      case 'E':
      case 'e':
        {
          val = Serial.parseInt();
          Serial.print(F( "pitch gain 0 180 "));
          TimerSetup.px = val ;
          Serial.println(TimerSetup.px );
          break;
        }
      case 'h':
      case 'H':
        {
          val = Serial.parseInt();
          Serial.print(F("Excessive pitch threshold (deg per sample) "));
          TimerSetup.PitchExThresh = val;
          Serial.println(TimerSetup.PitchExThresh);
          break;
        }
      case 'f':
      case 'F':
        {
          val = Serial.parseInt();
          Serial.print(F( "roll gain 0 180 "));
          TimerSetup.py = val ;
          Serial.println(TimerSetup.py );
          break;
        }
      case 'j':
      case 'J':
        {
          val = Serial.parseInt();
          Serial.print(F("Yaw rate threshold (deg/sec) "));
          TimerSetup.YawRateExThresh = val;
          Serial.println(TimerSetup.YawRateExThresh);
          break;
        }
      case 'q':
      case 'Q':
        {
          Serial.println(F("exit wihtout saving"));
          break;
        }
      case 's':
        {
          Serial.println(F("saving data wait"));
          saveData();
          Serial.println(F("saving data done"));
          break;
        }
      case 'r':
        {
          Serial.println("Refresh");
          //mpu6050.setGyroOffsets(TimerSetup.calX/100.0, TimerSetup.calY/100.0, TimerSetup.calZ/100.0); 
          menuValues();
          break;
        }
      case 'l':
      case 'L':
        {
          Serial.println(F("Reset lap count"));
          TimerSetup.LapCount = 0;
          Serial.println(TimerSetup.LapCount / 10.0);
          break;
        }
      case 'm':
      case 'M':
        {
          {
            float vf = Serial.parseFloat();
            // store LapLimit in tenths (0.1 lap units)
            unsigned long tenths = (unsigned long)(vf * 10.0f + 0.5f);
            TimerSetup.LapLimit = tenths;
            Serial.print(F("Set LapLimit (laps): "));
            Serial.println(((float)TimerSetup.LapLimit) / 10.0f);
          }
          break;
        }
      case 'k':
      case 'K':
        {
          Serial.print(F("Set maneuver gain rx (0-180): "));
          {
            unsigned int v = Serial.parseInt();
            if (v > 180) v = 180;
            TimerSetup.rx = (byte)v;
            Serial.print(F("Set rx: "));
            Serial.println(TimerSetup.rx);
          }
          break;
        }
              case 'x':
              case 'X':
        {
          val = Serial.parseInt();
          TimerSetup.calX = val;
          Serial.println(F("cal x "));
          Serial.println(TimerSetup.calX );

          break;
        }
                      case 'y':
                      case 'Y':
        {
          val = Serial.parseInt();
          TimerSetup.calY = val;
          Serial.println(F("cal y "));
          Serial.println(TimerSetup.calY );

          break; 
        }
                     case 'z':
                      case 'Z':
        {
          val = Serial.parseInt();
          TimerSetup.calZ = val;
          Serial.println(F("cal z "));
          Serial.println(TimerSetup.calZ );
          break;
        }


              case 'o':
        {
          Serial.print(F( "LED off "));
          digitalWrite(LED3,LOW);
          digitalWrite(LED4,LOW);
          digitalWrite(LED5,LOW);

          break;
        }
        case 'O':
        {
          Serial.print(F( "LED on "));
          digitalWrite(LED3,HIGH);
          digitalWrite(LED4,HIGH);
          digitalWrite(LED5,HIGH);

          break;
        }
        case 'G':
        case 'g':
        {
            setUpMPU();
            break;

      
        }
        case 'v':
        case 'V':
          {
            Serial.println(F("Factory reset EEPROM requested. Send 'Y' to confirm."));
            // clear any pending input
            while (Serial.available() > 0) Serial.read();
            while (Serial.available() == 0) { }
            char c = Serial.read();
            if (c == 'Y' || c == 'y')
            {
              int eeAddress = 0;
              TimerSetup.calX = 1;
              TimerSetup.calY = 1;
              TimerSetup.calZ = 1;
              TimerSetup.px = 1;
              TimerSetup.py = 1;
              TimerSetup.rx = 1;
              TimerSetup.ry = 1;
              TimerSetup.PitchExThresh = 40;
              TimerSetup.YawRateExThresh = 20;
              TimerSetup.LapCount = 0;
              TimerSetup.LapLimit = 0;
              TimerSetup.FlySpeed[0] = 120; TimerSetup.FlySpeed[1] = 140; TimerSetup.FlySpeed[2] = 180;
              TimerSetup.FlyTime[0] = 40000; TimerSetup.FlyTime[1] = 40000; TimerSetup.FlyTime[2] = 40000;
              TimerSetup.ArmTime[0] = 500; TimerSetup.ArmTime[1] = 500; TimerSetup.ArmTime[2] = 500;
              TimerSetup.accelTime[0] = 5000; TimerSetup.accelTime[1] = 5000; TimerSetup.accelTime[2] = 5000;
              EEPROM.put(eeAddress, TimerSetup);
              Serial.println(F("Factory reset complete — defaults written to EEPROM."));
              Serial.println(F("Run 'G' from the menu to recalibrate the MPU if needed."));
            }
            else
            {
              Serial.println(F("Factory reset cancelled."));
            }
            break;
          }
      }
    }
  }
}


void menuValues()
{
  read_giro();
  Serial.print(F(" a 1,2,3 Fly Speed 0-180   "));
  Serial.print (TimerSetup.FlySpeed[0]); Serial.print("  "); 
  Serial.print(TimerSetup.FlySpeed[1]); Serial.print("  ");
  Serial.println(TimerSetup.FlySpeed[2] ); 
  Serial.print(F(" b 1,2,3 Fly Time sec "));
  Serial.print(TimerSetup.FlyTime[0] ); Serial.print("  "); 
  Serial.print(TimerSetup.FlyTime[1] ); Serial.print("  ");
  Serial.println(TimerSetup.FlyTime[2] ); 
  Serial.print(F(" c 1,2,3 Arm Time sec "));  
  Serial.print(TimerSetup.ArmTime[0] ); Serial.print("  ");
  Serial.print(TimerSetup.ArmTime[1] );  Serial.print("  ");
   Serial.println(TimerSetup.ArmTime[2] );
  Serial.print(F(" d 1,2,3 accel Time msec "));
  Serial.print(TimerSetup.accelTime[0] ); Serial.print("  ");
  Serial.print(TimerSetup.accelTime[1] ); Serial.print("  ");
  Serial.println(TimerSetup.accelTime[2] );
  Serial.print(F(" L  Lap count "));
  Serial.println(((float)TimerSetup.LapCount) / 10.0f );
  Serial.print(F(" M  Lap limit "));
  Serial.println(((float)TimerSetup.LapLimit) / 10.0f );
  Serial.print(F(" P  Pos trim "));
  Serial.println(posTrim);
  Serial.print(F(" R  Maneuver rx "));
  Serial.println(TimerSetup.rx);
  Serial.print(F(" B  Maneuver boost "));
  Serial.println(maneuverBoost);
  Serial.print(F(" E  Pitch gain 0- 180 "));
  Serial.println(TimerSetup.px );
  Serial.print(F(" H  Excessive pitch threshold (deg per sample) "));
  Serial.println(TimerSetup.PitchExThresh );
  Serial.print(F(" F  Roll gain 0 -180 "));
  Serial.println(TimerSetup.py );
  Serial.print(F(" J  Yaw rate threshold (deg/sec) "));
  Serial.println(TimerSetup.YawRateExThresh );
  Serial.print(F(" X gyro cal X "));
  Serial.println(TimerSetup.calX );
  Serial.print(F(" Y gyro cal Y "));
  Serial.println(TimerSetup.calY );
  Serial.print(F(" Z gyro cal Z "));
  Serial.println(TimerSetup.calZ );
 // Serial.print(F(" X gyro value "));
 // Serial.println(iangleX );
 // Serial.print(F(" Y gyro val  "));
//  Serial.println(iangleY );
  
  
  Serial.println(F(" **************************"));
  Serial.println(F(" r refresh"));
  Serial.println(F(" s save"));
  Serial.println(F(" (O)n or (o)ff to test LEDs" ));
  Serial.println(F(" G run calibration of Gyro "));
  Serial.println(F(" q exit withiout saving"));
  Serial.println(F(" V Factory reset EEPROM (requires confirmation)"));
  
  Serial.println(F(" letter profile and value with a space between" ));
}





void terminal()
{
    
  banner();
  menuValues();
  getInput();
}