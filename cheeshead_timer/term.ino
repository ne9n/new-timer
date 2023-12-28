
#include <EEPROM.h>

void banner()
{
  Serial.flush();
  Serial.println ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
  Serial.println (" Cheesehead Timer");
  Serial.println (" A control line time and speed regulator");
  Serial.println (" https://github.com/ne9n");
  Serial.println (" Dave Siegler ne9n.dave@gmail.com");
  Serial.println ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

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
          Serial.print( "Set fly speed ");
          TimerSetup.FlySpeed = val;
          if (TimerSetup.FlySpeed > 180) {
            TimerSetup.FlySpeed = 180;
          }
          Serial.println(TimerSetup.FlySpeed);
          break;
        }
      case 'b':
      case 'B':
        {
          Serial.print( "Fly Time  secs is ");
          TimerSetup.FlyTime = val; 
          Serial.println(TimerSetup.FlyTime );
          break;
        }
      case 'c':
      case 'C':
        {
          Serial.print( "arm time in seconds is ");
          TimerSetup.ArmTime = val ;
          Serial.println(TimerSetup.ArmTime );
          break;
        }
      case 'D':
      case 'd':
        {
          Serial.print( "Accel time in mseconds ");
          TimerSetup.accelTime = val ;
          Serial.println(TimerSetup.accelTime );
          break;
        }
      case 'E':
      case 'e':
        {
          Serial.print( "pitch gain 0 180 ");
          TimerSetup.px = val ;
          Serial.println(TimerSetup.px );
          break;
        }
      case 'f':
      case 'F':
        {
          Serial.print( "roll gain 0 180 ");
          TimerSetup.py = val ;
          Serial.println(TimerSetup.py );
          break;
        }
      case 'q':
      case 'Q':
        {
          Serial.println("exit wihtout saving");
          break;
        }
      case 's':
        {
          Serial.println("saving data wait");
          saveData();
          Serial.println("saving data done");
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
          Serial.println("cal x ");
          Serial.println(TimerSetup.calX );

          break;
        }
                      case 'y':
                      case 'Y':
        {
          TimerSetup.calY = val;
          Serial.println("cal y ");
          Serial.println(TimerSetup.calY );

          break; 
        }
                     case 'z':
                      case 'Z':
        {
          TimerSetup.calZ = val;
          Serial.println("cal z ");
          Serial.println(TimerSetup.calZ );
          break;
        }


              case 'o':
        {
          Serial.print( "LED off ");
          digitalWrite(LED3,LOW);
          digitalWrite(LED4,LOW);
          digitalWrite(LED5,LOW);

          break;
        }
        case 'O':
        {
          Serial.print( "LED on ");
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
  Serial.print(" a Fly Speed 0-180 ");
  Serial.println(TimerSetup.FlySpeed );
  Serial.print(" b Fly Time sec ");
  Serial.println(TimerSetup.FlyTime );
  Serial.print(" c Arm Time sec ");  
  Serial.println(TimerSetup.ArmTime );
  Serial.print(" d accel Time msec ");
  Serial.println(TimerSetup.accelTime );
  Serial.print(" E  Pitch gain 0- 180 ");
  Serial.println(TimerSetup.px );
  Serial.print(" F  Roll gain 0 -180 ");
  Serial.println(TimerSetup.py );
  Serial.print(" X gyro cal X ");
  Serial.println(TimerSetup.calX );
  Serial.print(" Y gyro cal Y ");
  Serial.println(TimerSetup.calY );
  Serial.print(" Z gyro cal Z ");
  Serial.println(TimerSetup.calZ );
  Serial.print(" X gyro value ");
  Serial.println(angleX );
  Serial.print(" Y gyro val  ");
  Serial.println(angleY );
  
  
  Serial.println(" **************************");
  Serial.println(" r refresh");
  Serial.println(" s save");
  Serial.println(" (O)n or (o)ff to test LEDs" );
  Serial.println(" G run calibration of Gyro ");
  Serial.println(" q exit withiout saving");
  
  Serial.println(" command value no spaces");
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
