#ifndef GYRO_H
#define GYRO_H
//#include <ARDUINO.H>
#include <MPU6050_tockn.h> 
#include <Wire.h>


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

void speedGyro();
void mpu_setup();
// Evaluate high pitch and yaw events (refactored from read_giro)
void evaluateHighPitchYaw(int dx, int dy, int dz);

// Test/mocking helpers
void setMockMPU(bool v);
void setMockAngles(int x, int y, int z);
int getAngleX_read();
int getAngleY_read();
int getAngleZ_read();
void processSerialMPU();

// Test/mocking hooks (serial-driven)
void setMockMPU(bool v);
void setMockAngles(int x, int y, int z);
void processSerialMPU();
int getAngleX_read();
int getAngleY_read();
int getAngleZ_read();



#endif