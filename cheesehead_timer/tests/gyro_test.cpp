#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>

#include <EEPROM.h>
#include <MPU6050_tockn.h>
#include <Servo.h>
#include "cheesehead_timer.h"
#include "gyro.h"

param TimerSetup = {};
Servo esc;
unsigned long currentMillis = 0;
bool run_state = true;
speed_state ispeed_state = WAIT;
extern bool PitchEX;
extern bool YawEX;
extern bool YawLOW;
FakeWire Wire;
FakeEEPROM EEPROM;
FakeSerial Serial;

static int ds3State = HIGH;

unsigned long millis()
{
  return currentMillis;
}

int digitalRead(int)
{
  return ds3State;
}

void digitalWrite(int, int) {}
void delay(unsigned long duration) { currentMillis += duration; }
int FakeSerial::available() { return 0; }
String FakeSerial::readStringUntil(char) { return String(); }

void String::trim()
{
  const std::string whitespace = " \t\r\n";
  const std::size_t first = value_.find_first_not_of(whitespace);
  if (first == std::string::npos)
  {
    value_.clear();
    return;
  }
  value_ = value_.substr(first, value_.find_last_not_of(whitespace) - first + 1);
}

bool String::equalsIgnoreCase(const char *value) const
{
  if (value_.size() != std::string(value).size()) return false;
  for (std::size_t i = 0; i < value_.size(); ++i)
  {
    if (std::tolower(static_cast<unsigned char>(value_[i])) !=
        std::tolower(static_cast<unsigned char>(value[i])))
      return false;
  }
  return true;
}

bool String::startsWith(const char *value) const
{
  return value_.compare(0, std::string(value).size(), value) == 0;
}

unsigned int String::length() const
{
  return static_cast<unsigned int>(value_.size());
}

const char *String::c_str() const
{
  return value_.c_str();
}

void setSpeedState(speed_state newState)
{
  ispeed_state = newState;
}

static void expectEqual(const char *name, long expected, long actual)
{
  if (expected == actual) return;
  std::cerr << name << ": expected " << expected << ", got " << actual << '\n';
  std::exit(EXIT_FAILURE);
}

static void advanceSample(int x, int y, int z)
{
  currentMillis += 100;
  setMockAngles(x, y, z);
  processGyroSafety();
}

static void testConfiguredYawAxisCountsWholeLaps()
{
  TimerSetup.LapLimit = 0;
  ispeed_state = ARMED;
  run_state = true;
  setMockMPU(true);
  setMockAngles(0, 0, 0);
  currentMillis = 100;
  processGyroSafety();

  const int lapsToSimulate = 3;
  for (byte yawAxis = 0; yawAxis < 3; ++yawAxis)
  {
    TimerSetup.axisYaw = yawAxis;
    TimerSetup.axisPitch = (yawAxis + 1) % 3;
    TimerSetup.axisRoll = (yawAxis + 2) % 3;
    TimerSetup.LapCount = 0;
    advanceSample(0, 0, 0);

    for (int degree = 1; degree <= lapsToSimulate * 360; ++degree)
    {
      int rawAngles[3] = {0, 0, 0};
      rawAngles[yawAxis] = degree % 360;
      advanceSample(rawAngles[0], rawAngles[1], rawAngles[2]);
    }

    expectEqual("configured yaw-axis rotations count tenths of laps",
                lapsToSimulate * 10, static_cast<long>(TimerSetup.LapCount));
  }
}

static void testExcessiveYawFaultOnZAxis()
{
  TimerSetup.YawRateExThresh = 20;
  TimerSetup.PitchExThresh = 40;
  ispeed_state = ARMED;
  advanceSample(0, 0, 30);

  ispeed_state = FLY;
  run_state = true;
  ds3State = HIGH;
  advanceSample(0, 0, 30);
  advanceSample(0, 0, 33);
  advanceSample(0, 0, 36);

  expectEqual("excessive Z-axis yaw sets YawEX", 1, YawEX);
  expectEqual("excessive yaw clears run state", 0, run_state);
}

static void testLowYawFaultOnZAxis()
{
  ispeed_state = ARMED;
  for (int z = 35; z >= 0; --z)
  {
    advanceSample(0, 0, z);
  }

  ispeed_state = FLY;
  run_state = true;
  ds3State = HIGH;
  for (int sample = 0; sample < 5; ++sample)
  {
    advanceSample(0, 0, 0);
  }

  expectEqual("low Z-axis yaw sets YawLOW", 1, YawLOW);
  expectEqual("low yaw clears run state", 0, run_state);
}

static void testExcessivePitchFaultOnXAxis()
{
  TimerSetup.PitchExThresh = 5;
  ispeed_state = ARMED;
  run_state = true;
  advanceSample(0, 0, 0);
  advanceSample(6, 0, 0);
  advanceSample(12, 0, 0);

  expectEqual("excessive X-axis pitch sets PitchEX", 1, PitchEX);
  expectEqual("excessive pitch clears run state", 0, run_state);
}

int main()
{
  testConfiguredYawAxisCountsWholeLaps();
  testExcessiveYawFaultOnZAxis();
  testLowYawFaultOnZAxis();
  testExcessivePitchFaultOnXAxis();
  std::cout << "All gyro tests passed (configured yaw-axis laps, excessive yaw, low yaw, excessive pitch).\n";
  return EXIT_SUCCESS;
}
