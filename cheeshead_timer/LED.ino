/*
 * LED controller 
 * skip time but be a power of 2
 */

#include<NoDelay.h>

#define LED3 10
#define LED4 11
#define LED5 12


extern void ledTest();
 
int led_state = 0;

struct light
{
    byte pin;
    byte skip;
    bool invert;
    bool toggle;
};

struct light lights[3];

noDelay LEDtime(100);//Creats a noDelay varible set to 1000ms



void led_init()
{
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  lights[0].pin = LED3;
  lights[4].pin = LED4;
  lights[5].pin = LED5;
 
  

}





void ledUpdate()
{
  static byte ct;
  byte i,j;
  ct++;
  if(LEDtime.update())//Checks to see if set time has past
  { 
 
    for (i = 0; i<3; i++)
    {
      j= ct & 1;
      if (j=0 )
          digitalWrite(lights[i].pin,lights[i].toggle);
          if(lights[i].toggle == HIGH)
              lights[i].toggle = LOW;
          else
              lights[i].toggle = HIGH;
        
    }
  }
}

void ledTest()
{
   static bool x;  
    if (lights[0].toggle== HIGH)
  
      lights[0].toggle= LOW;
     
    else
      lights[0].toggle = HIGH;
      
    digitalWrite(lights[0].pin, lights[0].toggle);
        
    lights[0].toggle !=lights[0].toggle; 


 /*   for (i = 0; i<4; i++)
    {

          digitalWrite(lights[i].pin,lights[i].toggle*lights[i].invert);
          lights[i].toggle !=lights[i].toggle; 
        
    }*/

   /* digitalWrite(LED3, HIGH);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, HIGH);

    delay(200);
 
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, HIGH);
    digitalWrite(LED5, LOW);
*/
}
