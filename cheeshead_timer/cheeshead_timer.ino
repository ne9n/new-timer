#include <Servo.h>
#include <EEPROM.h>
#include <MPU6050_tockn.h>
#include <Wire.h>


// create servo object to control a servo
// state constants part of config values

// LIST OF THINGS TO DO
// START BUTTON no debounce
// save cal data manu
// mein for cal process


// acclermotoer support
// accel and decel ramps work but need a better method

typedef struct{
  unsigned int FlySpeed; // 0 to 180 for degrees of rotation of a servo
  unsigned int FlyTime; // air time in miliseconfs
  unsigned int ArmTime; // wait time in milisecnds
  unsigned int accelTime;
}time_t;

time_t cheze;
MPU6050 mpu6050(Wire);
Servo esc;

#define IncThrottle 1
#define BurpMax 180
#define BURPTIME 500
#define RDYTIME 5000

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

extern void terminal();
extern void speedState();
extern void gyro();
#define INCTIME 50



// definations
int maxThrottle = cheze.FlySpeed;
int state_tmr;
int curThrottle = 0; // current speed
int incTime = 0;
int led_state = 0;
int gyro_flag = false;
int skip;
int angleX;
int angleY;
int angleZ;


const int buttonPin = 7;

int fly_state = WAIT;
int inFlight = FLYING;




void setup()
{
  // attaches the servo on pin 9 to the servo object
  esc.attach(9, 1000, 2000);
  esc.write(curThrottle);
  int eeAddress = 0;   
  EEPROM.get(eeAddress, cheze );
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  terminal();
}





void loop()
{
  speedState();
   gyro();
 // ledUpdate();
}
