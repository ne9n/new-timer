/*
 * LED controller 
 * skip time but be a power of 2
 */


#include "led.h"


#define BUTTONPIN 13
//#define SW1 13
#define LED3 10
#define LED4 11
#define LED5 12


extern void ledTest();

 Led led3(LED3);
 Led led4(LED4);
 Led led5(LED5);
