#include <cstdlib>
#include <iostream>

#include <Servo.h>
#include "state_machine.h"

param TimerSetup = {};
Servo esc;
bool run_state = true;
speed_state ispeed_state = WAIT;
int iangleZ = 0;
int posTrim = 0;
int maneuverBoost = 0;
int sindex = 0;
bool PitchEX = false;
bool YawEX = false;
bool YawLOW = false;
extern unsigned long state_timer[];

static unsigned long mockMillis = 0;
static int ledState = -1;

unsigned long millis()
{
  return mockMillis;
}

void set_led(int state)
{
  ledState = state;
}

static void expectEqual(const char *name, int expected, int actual)
{
  if (expected == actual) return;
  std::cerr << name << ": expected " << expected << ", got " << actual << '\n';
  std::exit(EXIT_FAILURE);
}

static void testFlyThrottleAndModifiers()
{
  TimerSetup.FlySpeed[0] = 100;
  TimerSetup.autoSpeedPerMin = 0;
  posTrim = 20;
  maneuverBoost = 15;
  run_state = true;
  PitchEX = false;
  YawEX = false;
  YawLOW = false;
  mockMillis = 100;
  setSpeedState(FLY);

  speedState();

  expectEqual("FLY throttle with modifiers", 135, esc.value);
  expectEqual("LED state follows flight state", FLY, ledState);
}

static void testThrottleClamping()
{
  TimerSetup.FlySpeed[0] = 170;
  posTrim = 20;
  maneuverBoost = 20;
  run_state = true;
  setSpeedState(FLY);
  speedState();
  expectEqual("upper throttle clamp", MAX_SPEED, esc.value);

  TimerSetup.FlySpeed[0] = 10;
  posTrim = -20;
  maneuverBoost = 0;
  setSpeedState(FLY);
  speedState();
  expectEqual("lower throttle clamp", 0, esc.value);
}

static void testBurpThrottle()
{
  run_state = true;
  posTrim = 0;
  maneuverBoost = 0;
  setSpeedState(BURP);

  speedState();

  expectEqual("BURP throttle", BurpMax, esc.value);
}

static void testRunStateIsAnAbsoluteCutoff()
{
  TimerSetup.FlySpeed[0] = 150;
  posTrim = 10;
  maneuverBoost = 10;
  run_state = false;
  setSpeedState(FLY);

  speedState();

  expectEqual("run-state safety cutoff", 0, esc.value);
}

static void testFlightSafetyFlagStopsTheMotor()
{
  TimerSetup.FlySpeed[0] = 150;
  run_state = true;
  PitchEX = true;
  YawEX = false;
  YawLOW = false;
  setSpeedState(FLY);

  speedState();

  expectEqual("pitch safety flag clears run state", 0, run_state);
  expectEqual("pitch safety flag stops the motor", 0, esc.value);
  PitchEX = false;
}

static void testRampDownStopsTheMotor()
{
  run_state = true;
  setSpeedState(RAMPDWN);

  speedState();

  expectEqual("ramp-down clears run state", 0, run_state);
  expectEqual("ramp-down stops the motor", 0, esc.value);
}

static void testStateTimeoutBoundary()
{
  mockMillis = 0;
  setSpeedState(WAIT);
  mockMillis = state_timer[WAIT];
  expectEqual("state does not advance at timeout boundary", 0, check_state());
  expectEqual("state remains WAIT at timeout boundary", WAIT, ispeed_state);

  mockMillis++;
  expectEqual("state advances after timeout", 1, check_state());
  expectEqual("WAIT advances to ARMED", ARMED, ispeed_state);
}

int main()
{
  testFlyThrottleAndModifiers();
  testThrottleClamping();
  testBurpThrottle();
  testRunStateIsAnAbsoluteCutoff();
  testFlightSafetyFlagStopsTheMotor();
  testRampDownStopsTheMotor();
  testStateTimeoutBoundary();
  std::cout << "All state-machine tests passed.\n";
  return EXIT_SUCCESS;
}
