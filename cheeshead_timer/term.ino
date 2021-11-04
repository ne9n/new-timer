
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
  EEPROM.put(eeAddress, cheze );
}
void get1Input()
{
  char menu = ' ';
  unsigned int x = 0;
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
    x = Serial.parseInt();
    Serial.print(menu);
    Serial.println(x);
    
    switch (menu)
    {
      case 'a':
      case 'A':
        {
          Serial.print( "Set fly speed ");
          cheze.FlySpeed = x;
          if (cheze.FlySpeed > 180) {
            cheze.FlySpeed = 180;
          }
          Serial.println(cheze.FlySpeed);
          break;
        }
      case 'b':
      case 'B':
        {
          Serial.print( "Fly Time  secs is ");
          cheze.FlyTime = x; 
          Serial.println(cheze.FlyTime );
          break;
        }
      case 'c':
      case 'C':
        {
          Serial.print( "arm time in seconds is ");
          cheze.ArmTime = x ;
          Serial.println(cheze.ArmTime );
          break;
        }
      case 'D':
      case 'd':
        {
          Serial.print( "Accel time in mseconds ");
          cheze.accelTime = x ;
          Serial.println(cheze.accelTime );
          break;
        }
      case 'E':
      case 'e':
        {
          Serial.print( "pitch gain 0 180 ");
          cheze.k1 = x ;
          Serial.println(cheze.k1 );
          break;
        }
      case 'f':
      case 'F':
        {
          Serial.print( "roll gain 0 180 ");
          cheze.k2 = x ;
          Serial.println(cheze.k2 );
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
          mpu6050.setGyroOffsets(cheze.calX/100.0, cheze.calY/100.0, 0); 
          menuValues();
          break;
        }
              case 'x':
              case 'X':
        {
          cheze.calX = x;
          Serial.println("cal x ");
          Serial.println(cheze.calX );

          break;
        }
                      case 'y':
                      case 'Y':
        {
          cheze.calY = x;
          Serial.println("cal y ");
          Serial.println(cheze.calY );

          break; 
        }
                     case 'z':
                      case 'Z':
        {
         }


              case 'o':
        {
          Serial.print( "LED off ");
          digitalWrite(3,LOW);
          digitalWrite(4,LOW);
          digitalWrite(5,LOW);

          break;
        }
        case 'O':
        {
          Serial.print( "LED on ");
          digitalWrite(3,HIGH);
          digitalWrite(4,HIGH);
          digitalWrite(5,HIGH);

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
  Serial.println(cheze.FlySpeed );
  Serial.print(" b Fly Time sec ");
  Serial.println(cheze.FlyTime );
  Serial.print(" c Arm Time sec ");  
  Serial.println(cheze.ArmTime );
  Serial.print(" d accel Time msec 1000= 1 ");
  Serial.println(cheze.accelTime );
  Serial.print(" E  Pitch gain 0- 180 ");
  Serial.println(cheze.k1 );
  Serial.print(" F  Roll gain 0 -180 ");
  Serial.println(cheze.k2 );
  Serial.print(" X gyro cal X ");
  Serial.println(cheze.calX );
  Serial.print(" Y gyro cal Y ");
  Serial.println(cheze.calY );
  Serial.print(" X gyro value ");
  Serial.println(angleX );
  Serial.print(" Y gyro val  ");
  Serial.println(angleY );
  
  
  Serial.println(" **************************");
  Serial.println(" r refresh");
  Serial.println(" s save");
  Serial.println(" O or o to tesr LED's" );
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
