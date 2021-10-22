# new-timer
state driven control line; line motor timer

this is a devcie used for electric power control line airplanes 
The idea is to change the speed of the motor based on the ptich and roll of the airplane.  

Target is an arduino and usees SPI connected giro to measure attitude.  
it hsa on board storage of setup data, and serial termaina based menu.
it usese the servo lib and task scheduler

Wire is for SPI connection 

#include <Servo.h>
#include <EEPROM.h>
#include <MPU6050_tockn.h>
#include <Wire.h>

the speed function works like this 

speed command =   speed set point + K1 * sin ( pitch angle) + K2 *abs (sine (roll angle))

K1 and K2 are constands in save in the program.  
the sensor is set up so that the pitch and roll nagles are zero when in level flight.  

the sine function will be implimeted as a lookup table 
the system is controlled by a state machine and stated by a push button

Waiting--> ramp up-->  flying--> ranp down --> landing --> back to waiting
