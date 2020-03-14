



void gyro()
{
  mpu6050.update();
  angleX = mpu6050.getAngleX();
  angleY = mpu6050.getAngleY();
  angleZ = mpu6050.getAngleZ();
  //posTrim = (K1*(angleX+OFFSET1)*1024)/1024 + (K2*angleY*1024)/1024;
  posTrim = cheze.k1 * sin(angleX * 1000.0  / 57296.0) + cheze.k2 * abs(sin(angleY * 1000.0  / 57296.0));
  //  Serial.println(angleX);
  //  Serial.println(posTrim);
}
