#include <Servo.h>
#include <EEPROM.h>
#include <MPU6050_tockn.h>
#include <Wire.h>

// Dave Siegler ne9n.dave@gmail.com
// This is a timer that steps through a seguence of speeds
// to control the throttle poistion of a control line airplane
// in includes user set up values 
// LED;s sho state of the time 
// an MUP is used to trim the throttle based on 
// pitch and roll values of te airplane
// create servo object to control a servo
// state constants part of config values
// 

// acclermotoer support using MPU6050 tockn laibrat

// SHOT TERM
// 1) improve LED , mathe the flashine 
// 2) clean up menu
// 3) range checl control functions
// 4) better burp
// 5) Draw it up and write docs 

// LONG TERM
// 1) RC transmitter interface
// 2) Add acceleration boost to control function
// 3) table based lookup for control to replace sin()

typedef struct {
  unsigned int FlySpeed; // 0 to 180 for degrees of rotation of a servo
  unsigned int FlyTime; // air time in seconds
  unsigned int ArmTime; // wait time in seconds
  unsigned int accelTime; // in msec
  int calX;
  int calY;
  int calZ;
  byte k1;
  byte k2;
} time_t;

time_t cheze;
MPU6050 mpu6050(Wire);
Servo esc;

#define IncThrottle 1
#define BurpMax 180
#define BURPTIME 500
#define RDYTIME 5000
#define LANDTIME 0x1FF
#define INCTIME 50

#define FLASHON 500
#define FLASHOFF 500

#define WAIT 0
#define ARMED 1
#define TAKEOFF 2
#define FLY 3
#define RAMPDWN 4
#define LAND 5

// sub state for flyng
#define FLYING 0
#define BURP 1
#define RDYLAND 2
#define BUTTONPIN 7
#define SW1 8
#define SERVO 9
#define LED3 3
#define LED4 4
#define LED5 5


extern void terminal();
extern void speedState();
extern void gyro();
extern void printDebug();


// definations
int maxThrottle = cheze.FlySpeed;
unsigned long state_tmr;
int curThrottle = 0; // current speed
int incTime = 0;
int led_state = 0;
int gyro_flag = false;
unsigned int skip;
int angleX;
int angleY;
int angleZ;
int posTrim;



// state variables 
int fly_state = WAIT;
int inFlight = FLYING;

unsigned long currentMillis = millis();
 


void setup()
{
  // attaches the servo on pin 9 to the servo object
  esc.attach(SERVO, 1000, 2000);
  esc.write(curThrottle);
  int eeAddress = 0;
  EEPROM.get(eeAddress, cheze );
//  mpu6050.setGyroOffsets(calX, calY, calZ);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(BUTTONPIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  Serial.begin(19200);
  Wire.begin();
  mpu6050.begin();
 // mpu6050.calcGyroOffsets(true);
 mpu6050.setGyroOffsets(cheze.calX/100.0, cheze.calY/100.0, cheze.calZ/100.0);
}


void printDebug(void)
{
    if (skip < 3)
  {
    //      Serial.print("angleX : ");
    //      Serial.print(angleX);
    //      Serial.print("\tangleY : ");
    //      Serial.print(angleY);
    //      Serial.print("\tangleZ : ");
    //      Serial.print(angleZ);
          Serial.print("\t posTrim : ");
          Serial.print(posTrim);
          Serial.print("\t throttle : ");
          Serial.print(curThrottle);
    
//            Serial.print(" fly_state : ");
//            Serial.print(fly_state);
//            Serial.print(" currentMillis -state_tmr : ");
//            Serial.print(currentMillis-state_tmr);

    Serial.println();
  }
  else
  {
    if (skip > 500)
    {
      skip = 500;
    }
    skip--;
  }

}


void loop()
{
  speedState();
  printDebug();
  gyro();
  // ledUpdate();
}
