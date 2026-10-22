#ifndef TESTS_FAKES_ARDUINO_H
#define TESTS_FAKES_ARDUINO_H

#include <math.h>
#include <string>

using byte = unsigned char;

#define F(value) value
#define PROGMEM
#define HIGH 1
#define LOW 0

unsigned long millis();
int digitalRead(int pin);
void digitalWrite(int pin, int value);
void delay(unsigned long duration);

class String
{
public:
  String() = default;
  String(const char *value) : value_(value) {}

  void trim();
  bool equalsIgnoreCase(const char *value) const;
  bool startsWith(const char *value) const;
  unsigned int length() const;
  const char *c_str() const;

private:
  std::string value_;
};

class FakeSerial
{
public:
  int available();
  String readStringUntil(char delimiter);

  template <typename T>
  void print(const T &) {}

  template <typename T>
  void println(const T &) {}

  void println() {}
};

extern FakeSerial Serial;

#endif
