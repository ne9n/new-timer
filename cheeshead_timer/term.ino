
#include <EEPROM.h>



void termSetup(void)
{
}


  

void banner()
{
  Serial.flush();
  Serial.println (" https://github.com/ne9n               ");
  Serial.println (" Dave Siegler ne9n.dave@gmail.com      ");
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
          cheze.accelTimeMs = x ;
          Serial.println(cheze.accelTimeMs );
          break;
        }
      case 'E':
      case 'e':
        {
          Serial.print( "pitch gain x 0 180 ");
          cheze.px = x ;
          Serial.println(cheze.px );
          break;
        }
      case 'f':
      case 'F':
        {
          Serial.print( "roll gain 0 x 180 ");
          cheze.rx = x ;
          Serial.println(cheze.rx );
          break;
        }
      case 'H':
      case 'h':
        {
          Serial.print( "pitch gain y 0 180 ");
          cheze.py = x ;
          Serial.println(cheze.py );
          break;
        }
      case 'i':
      case 'I':
        {
          Serial.print( "roll gain 0 y 180 ");
          cheze.ry = x ;
          Serial.println(cheze.ry );
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
          led3.off();
          led4.off();
          led5.off();
          break;
        }
        case 'O':
        {
          Serial.print( "LED on ");
          led3.on();
          led4.on();
          led5.on();

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
  Serial.print(" a Fly Speed 0-180 "); Serial.println(cheze.FlySpeed );
  Serial.print(" b Fly Time sec    "); Serial.println(cheze.FlyTime );
  Serial.print(" c Arm Time sec    "); Serial.println(cheze.ArmTime );
  Serial.print(" d accel Time msec "); Serial.println(cheze.accelTimeMs );
  Serial.print(" E xaxis Pitch gain"); Serial.println(cheze.px );
  Serial.print(" F xaxis Roll gain "); Serial.println(cheze.rx );
  
  Serial.print(" H yaxis Pitch gain"); Serial.println(cheze.px );
  Serial.print(" I yaxis Roll gain "); Serial.println(cheze.rx );
  Serial.print(" X gyro cal X      "); Serial.println(cheze.calX );
  Serial.print(" Y gyro cal Y      "); Serial.println(cheze.calY );
  Serial.print(" X gyro value      "); Serial.println(angleX );
  Serial.print(" Y gyro val        "); Serial.println(angleY );
  
  
  Serial.println(" **************************");
  Serial.println(" r refresh");
  Serial.print(" s save");
  Serial.print(" O or o  LED's" );
  Serial.print(" G Gyro Cal");
  Serial.println(" q exit withiout saving");
  
  Serial.println(" Type command value no spaces");
}


void read_giro()
{
  angleX = mpu6050.getAngleX();
  angleY = mpu6050.getAngleY();

}


void terminal()
{
    
  banner();
  //Vt_100_term();
  menuValues();
  get1Input();
}
