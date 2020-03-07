int posTrim;
#define K1 0.8
#define K2 0.5
void gyro()
{
  mpu6050.update();
  angleX = mpu6050.getAngleX();
  angleY = mpu6050.getAngleY();
  angleZ = mpu6050.getAngleZ();
  posTrim = (K1*angleX*1024) /1024 + (K2*angleY*1024)/1024;
  
//  if (skip < 3)
//    {
//      skip = 500;
//      Serial.print("angleX : ");
//      Serial.print(angleX);
//      Serial.print("\tangleY : ");
//      Serial.print(angleY);
//      Serial.print("\tangleZ : ");
//      Serial.print(angleZ);
//      Serial.print("\t posTrim : ");
//      Serial.print(posTrim);
//      Serial.print("\t throttle : ");
//      Serial.print(cheze.FlySpeed + posTrim);
//
//      Serial.println();
//  }
//  else{
//    if (skip > 500)
//    {skip = 500;}
//    skip--;
//  }
  return(posTrim);
}
