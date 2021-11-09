#include "led.h"
int cMillis = millis();

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
void Led::flashF()
{
  cMillis = millis();
  if(cMillis & (1 << 5))
        digitalWrite(pin, LOW);
        
    else
        digitalWrite(pin,HIGH);          
}

void Led::flashS()
{
  cMillis = millis();
  if(cMillis & (1 << 9))
        digitalWrite(pin, LOW);
        
    else
        digitalWrite(pin,HIGH);          
}
