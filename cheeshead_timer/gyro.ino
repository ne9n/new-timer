/*
 *   Need a look up table for SIN  
 */
// 500 == 0

#define SIN_TBL_SZ 36 
#define OFFSET 400
const int sinetbl[37]= {
500,587,671,750,821,883,933,970,
992,1000,992,970,933,883,821,750,
671,587,500,413,329,250,179,117,
67,30,8,0,8,30,67,117,
179,250,329,413,500};

void gyro()
{
  int pitch;
  int roll;
  int sx;
  int sy;
  int ax;
  int ay;
  mpu6050.update();
  angleX = mpu6050.getAngleX();
  angleY = mpu6050.getAngleY();
  ax = angleX/ 10;
  ay = angleY/ 10;
  if (ax<0 || ax > 36)
  {
    ax= 36-abs(ax);
  }

  
  if (ay<0 || ay > 36)
  {
    ay= 36-abs(ay);
  }
  

    sx = sinetbl[ax] - OFFSET;
    sy = sinetbl[ay] - OFFSET;
    //Serial.print ("/tsx");
    //Serial.print (sx);
    //Serial.print ("/tsy");
    //Serial.println(sy);
   
    
 // sx = sin(angleX * 1000.0  / 57296.0);
 // sy = sin(angleY * 1000.0  / 57296.0);
  pitch = cheze.px * sx+cheze.py * sy;
  roll = cheze.rx *sx +cheze.ry*sy;
  posTrim =  pitch + abs(roll);

}
