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
  angleX = mpu6050.getAngleX()/10;
  angleY = mpu6050.getAngleY()/10;

// only 36 elements in sine table use anfle /10 as index
  if (angleX<0 || angleX > 36)
  {
    angleX= 36-abs(angleX);
  }

  
  if (angleY<0 || angleY > 36)
  {
    angleY= 36-abs(angleY);
  }
  

    sinX = sinetbl[angleX] - OFFSET;
    siny = sinetbl[angleY] - OFFSET;
    pitch = TimerSetup.px * sinX+TimerSetup.py * siny;
    roll = TimerSetup.rx *sinX +TimerSetup.ry*siny;

    pitch = TimerSetup.px * sinX+TimerSetup.py * siny;
    roll = TimerSetup.rx *sinX +TimerSetup.ry*siny;
    posTrim =  pitch + abs(roll);
 
}
