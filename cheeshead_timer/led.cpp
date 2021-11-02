#include "led.h"
#include<NoDelay.h>

noDelay LEDtime(500);//Creats a noDelay varible set to 1000ms


Led::Led(byte pin) {
  // Use 'this->' to make the difference between the
  // 'pin' attribute of the class and the 
  // local variable 'pin' created from the parameter.
  this->pin = pin;
  init();
}
void Led::init() {
  pinMode(pin, OUTPUT);
  // Always try to avoid duplicate code.
  // Instead of writing digitalWrite(pin, LOW) here,
  // call the function off() which already does that
  off();
}
void Led::on() {
  digitalWrite(pin, HIGH);
}
void Led::off() {
  digitalWrite(pin, LOW);
}
void Led::flash()
{
  if(LEDtime.update())//Checks to see if set time has past
  {
    if (digitalRead(pin))
        digitalWrite(pin, LOW);
    else
        digitalWrite(pin,HIGH);
  }
}
