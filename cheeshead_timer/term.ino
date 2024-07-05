
#include <EEPROM.h>

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

void saveData()
{

  int eeAddress = 0;   
  EEPROM.put(eeAddress, TimerSetup );
}
void get1Input()
{
  char menu = ' ';
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
    val = Serial.parseInt();
    Serial.print(menu);
    Serial.println(val);
    
    switch (menu)
    {
      case 'a':
      case 'A':
        {
          Serial.print(F( "Set fly speed "));
          TimerSetup.FlySpeed = val;
          if (TimerSetup.FlySpeed > MAX_SPEED) 
          {
            TimerSetup.FlySpeed = MAX_SPEED;
          }
          Serial.println(TimerSetup.FlySpeed);
          break;
        }
      case 'b':
      case 'B':
        {
          Serial.print(F( "Fly Time  secs is "));
          TimerSetup.FlyTime = val;
          state_timer[FLY] = TimerSetup.FlyTime; 
          Serial.println(TimerSetup.FlyTime );
          break;
        }
      case 'c':
      case 'C':
        {
          Serial.print(F( "arm time in seconds is "));
          TimerSetup.ArmTime = val ;
       //   speed_s[ARMED] = TimerSetup.ArmTime; 
          Serial.println(TimerSetup.ArmTime );
          break;
        }
      case 'D':
      case 'd':
        {
          Serial.print(F( "Accel time in mseconds "));
          TimerSetup.accelTime = val ;
          state_timer[TAKEOFF_RAMP] = TimerSetup.accelTime; 
          Serial.println(TimerSetup.accelTime );
          break;
        }
      case 'E':
      case 'e':
        {
          Serial.print(F( "pitch gain 0 180 "));
          TimerSetup.px = val ;
          Serial.println(TimerSetup.px );
          break;
        }
      case 'f':
      case 'F':
        {
          Serial.print(F( "roll gain 0 180 "));
          TimerSetup.py = val ;
          Serial.println(TimerSetup.py );
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
          mpu6050.setGyroOffsets(TimerSetup.calX/100.0, TimerSetup.calY/100.0, TimerSetup.calZ/100.0); 
          menuValues();
          break;
        }
              case 'x':
              case 'X':
        {
          TimerSetup.calX = val;
          Serial.println(F("cal x "));
          Serial.println(TimerSetup.calX );

          break;
        }
                      case 'y':
                      case 'Y':
        {
          TimerSetup.calY = val;
          Serial.println(F("cal y "));
          Serial.println(TimerSetup.calY );

          break; 
        }
                     case 'z':
                      case 'Z':
        {
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
            mpu6050.calcGyroOffsets(true);
            break;

        }
    }
  }
}


void menuValues ()
{
  read_giro();
  Serial.print(F(" a Fly Speed 0-180 "));
  Serial.println(TimerSetup.FlySpeed );
  Serial.print(F(" b Fly Time sec "));
  Serial.println(TimerSetup.FlyTime );
  Serial.print(F(" c Arm Time sec "));  
  Serial.println(TimerSetup.ArmTime );
  Serial.print(F(" d accel Time msec "));
  Serial.println(TimerSetup.accelTime );
  Serial.print(F(" E  Pitch gain 0- 180 "));
  Serial.println(TimerSetup.px );
  Serial.print(F(" F  Roll gain 0 -180 "));
  Serial.println(TimerSetup.py );
  Serial.print(F(" X gyro cal X "));
  Serial.println(TimerSetup.calX );
  Serial.print(F(" Y gyro cal Y "));
  Serial.println(TimerSetup.calY );
  Serial.print(F(" Z gyro cal Z "));
  Serial.println(TimerSetup.calZ );
  Serial.print(F(" X gyro value "));
  Serial.println(angleX );
  Serial.print(F(" Y gyro val  "));
  Serial.println(angleY );
  
  
  Serial.println(F(" **************************"));
  Serial.println(F(" r refresh"));
  Serial.println(F(" s save"));
  Serial.println(F(" (O)n or (o)ff to test LEDs" ));
  Serial.println(F(" G run calibration of Gyro "));
  Serial.println(F(" q exit withiout saving"));
  
  Serial.println(F(" command value no spaces"));
}


void read_giro()
{
  angleX = mpu6050.getAngleX();
  angleY = mpu6050.getAngleY();

}


void terminal()
{
    
  banner();
  menuValues();
  get1Input();
}
