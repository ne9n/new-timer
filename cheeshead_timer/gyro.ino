/*
 *   Need a look up table for SIN  
 */
 



void gyro()
{
  mpu6050.update();
  angleX = mpu6050.getAngleX();
  angleY = mpu6050.getAngleY();
  posTrim = cheze.k1 * sin(angleX * 1000.0  / 57296.0) + cheze.k2 * abs(sin(angleY * 1000.0  / 57296.0));

}
