#include <unity.h>

// include project headers
#include "state_machine.h"
#include "cheesehead_timer.h"
// include stubs
#include <Servo.h>

// Provide test stubs / globals expected by state_machine.cpp
unsigned long test_millis = 0;

// Globals from `state_machine.cpp`
extern unsigned long currentMillis;
extern unsigned long previousMillis;
extern speed_state ispeed_state;
extern unsigned long state_timer[];
extern bool run_state;
extern int curThrottle;
extern int iangleZ;
extern int posTrim;
extern int maneuverBoost;

// Provide TimerSetup instance used by state_machine
param TimerSetup;

// Provide missing globals referenced by state_machine.cpp
bool run_state = false;
speed_state ispeed_state = WAIT;
int iangleZ = 0;
int posTrim = 0;
int maneuverBoost = 0;
int lastEscWrite = -1; // used by Servo stub
// Servo instance (stub defined in test/include/Servo.h)
Servo esc;
bool PitchEX = false;
bool YawEX = false;
bool YawLOW = false;

// Minimal stub for LED helper used in state_machine
void set_led(int) {}

// `millis()` is provided by test/include/Arduino.h

void test_check_state_advances()
{
  // set previous to 0 and ensure current > state_timer for WAIT
  previousMillis = 0;
  test_millis = state_timer[WAIT] + 10;
  ispeed_state = WAIT;
  bool changed = check_state();
  TEST_ASSERT_TRUE(changed);
  TEST_ASSERT_EQUAL_INT((int)ARMED, (int)ispeed_state);
}

void test_speedstate_wait()
{
  ispeed_state = WAIT;
  posTrim = 0;
  maneuverBoost = 0;
  speedState();
  TEST_ASSERT_EQUAL_INT(25, curThrottle);
  // esc.write should have been called with same value
  TEST_ASSERT_EQUAL_INT(25, lastEscWrite);
}

void test_speedstate_fly_auto_increase()
{
  // enable auto increase and verify throttle grows over time
  TimerSetup.FlySpeed[0] = 100;
  TimerSetup.autoSpeedPerMin = 60; // 60% per minute
  setSpeedState(FLY);
  // advance one minute
  test_millis += 60000;
  ispeed_state = FLY;
  speedState();
  // extra should be ~60% of 100 = 60, so throttle 160
  TEST_ASSERT_TRUE(curThrottle >= 160);
  TEST_ASSERT_EQUAL_INT(curThrottle, lastEscWrite);
}

int main()
{
  UNITY_BEGIN();
  RUN_TEST(test_check_state_advances);
  RUN_TEST(test_speedstate_wait);
  RUN_TEST(test_speedstate_fly_auto_increase);
  return UNITY_END();
}
