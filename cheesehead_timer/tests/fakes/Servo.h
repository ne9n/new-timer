#ifndef TESTS_FAKES_SERVO_H
#define TESTS_FAKES_SERVO_H

class Servo
{
public:
  int value = -1;

  void write(int nextValue)
  {
    value = nextValue;
  }
};

#endif
