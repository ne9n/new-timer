#ifndef TESTS_FAKES_MPU6050_TOCKN_H
#define TESTS_FAKES_MPU6050_TOCKN_H

#include <Arduino.h>

class FakeWire
{
};

extern FakeWire Wire;

class MPU6050
{
public:
  explicit MPU6050(FakeWire &) {}

  void begin() {}
  void update() {}
  void setGyroOffsets(float, float, float) {}
  int getAngleX() const { return 0; }
  int getAngleY() const { return 0; }
  int getAngleZ() const { return 0; }
};

#endif
