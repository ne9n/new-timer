
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
  char menu = " ";
  unsigned int x = 0;
  while (menu != 'q' )
  {
    while (Serial.available() > 0)
    {
      Serial.read();  // clear the buffer
    }
    while (Serial.available() == 0) {} // wait for key
    menu = Serial.read();             // get slection
    x = Serial.parseInt();
    Serial.print(menu);
    Serial.println(x);
    switch (menu)
    {
      case 'a':
      case 'A':
        {
          Serial.print( "Set fly speed %");
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
          Serial.print( "Fly Time mseconds secs is ");
          cheze.FlyTime = x 
          Serial.println(cheze.FlyTime );
          break;
        }
      case 'c':
      case 'C':
        {
          Serial.print( "arm time in mseconds is ");
          cheze.ArmTime = x ;
          Serial.println(cheze.ArmTime );
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
          menuValues();
          break;
        }
    }
  }
}



void menuValues ()
{
  Serial.print(" a Fly Speed 0-180   ");
  Serial.println(cheze.FlySpeed );
  Serial.print(" b Fly Time msec     ");
  Serial.println(cheze.FlyTime );
  Serial.print(" c Arm Time msec     ");  
  Serial.println(cheze.ArmTime );
  Serial.print(" d accel Time msec   ");
  Serial.println(cheze.acelTime );

  Serial.println(" *********************");
  Serial.println(" r refresh");
  Serial.println(" s save");
  Serial.println(" q exit withiout saving");

  
  Serial.println(" command value no spaces");
}



void terminal()
{
  banner();
  menuValues();
  get1Input();
}
