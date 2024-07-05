/*
 *   Need a look up table for SIN  
 */
 #define SIN_TBL_SZ 36 
#define OFFSET 400
const int sinetbl[37] PROGMEM= {
500,587,671,750,821,883,933,970,
992,1000,992,970,933,883,821,750,
671,587,500,413,329,250,179,117,
67,30,8,0,8,30,67,117,
179,250,329,413,500};




void speedGyro()
{
  int pitch;
  int roll;
  int sinX;
  int siny;
 
  mpu6050.update();

  angleX = mpu6050.getAngleX();
  //Serial.print("angleX : ");      Serial.print(angleX);
  angleY = mpu6050.getAngleY();
  //Serial.print("angleY : ");      Serial.print(angleY);
 angleZ = mpu6050.getAngleZ();
  //Serial.print("angleZ : ");      Serial.print(angleZ);

/*  to do this is fast sinetable look up angle has an issues  
angleX = (uint8_t) angleX/10 % 36;
if (angleX <0)
{ 
  angleX = 36- angleX;
}
angleY = (uint8_t) angleY/10 % 36;
if (angleY <0)
{ 
  angleY = 36- angleY;
}

   pitch = TimerSetup.px * sinetbl[angleX]+ TimerSetup.py * sinetbl[angleY];
   roll = TimerSetup.rx *sinetbl[angleX] +TimerSetup.ry*sinetbl[angleY];
*/

/* watch out deg to radians */
/* controller lays flat*/

    pitch = TimerSetup.px * sin(angleX*3.14/180)/* + TimerSetup.py * cos(angleY)*/;
    roll = TimerSetup.ry*cos(angleY*3.14/180); /* offset for profile mounting */

/* controller mounts on a profile mount  z is pithc x i roll plus 90 */
   /* pitch = TimerSetup.px * sin(angleZ*3.14/180);
    roll = TimerSetup.ry*cos(angleY*3.14/180);
*/

 //   Serial.print(F("pitch :"));  Serial.print(pitch);Serial.print(F(","));
 //   Serial.print(F("roll:")); Serial.print(roll);Serial.print(F(","));
 //   Serial.print(F("\n"));

    posTrim =  pitch ;

    /* limit trim from 0 to max*/
    if (posTrim <0)
      posTrim = 0;
    else if(posTrim > 180)
      posTrim = 180;  
}
