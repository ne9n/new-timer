/*
 * LED controller 
 * skip time but be a power of 2
 */

#include<NoDelay.h>
#include "Led.h"
#include "Button.h"

#define LED3 10
#define LED4 11
#define LED5 12


extern void ledTest();
 
int led_state = 0;

struct light
{
    byte pin;
    byte skip;
    bool enable;
    bool toggle;
};

struct light lights[3];

noDelay LEDtime(250);//Creats a noDelay varible set to 1000ms



void led_init()
{
 
 Led led3(LED3);
 Led led4(LED4);
 Led led5(LED5);
 /* change this to external button*/
 Button button1(BUTTON_PIN); 
  

}





void ledUpdate()
{
  static byte ct;
  byte i;
  if(LEDtime.update())//Checks to see if set time has past
  {
   /* this jsut has flashing updates from LED object */
  }
 
}

void ledTest()
{

}
