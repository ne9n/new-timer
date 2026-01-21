#include <unity.h>

// Use production headers for types
#include "cheesehead_timer.h"

// Minimal test globals
unsigned long test_millis = 0;
int test_digital_read = HIGH;

// Provide TimerSetup used by the implementation
param TimerSetup;

// Globals mirrored from production code (we test logic in local function)
bool run_state = true;
speed_state ispeed_state = WAIT;
int iangleZ = 0;

// Local implementation of the evaluateHighPitchYaw logic for unit testing.
// This mirrors the logic in gyro.cpp but keeps state local to the test
void evaluateHighPitchYaw_local(int dx, int dy, int dz)
{
  static int pitchSetCounter = 0;
  static int pitchClearCounter = 0;
  const int PITCH_SET_N = 2;
  const int PITCH_CLEAR_M = 5;

  static int yawSlowCount = 0;
  static int yawHighCounter = 0;
  static int yawLowCounter = 0;
  const int YAW_HIGH_N = 2;
  const int YAW_LOW_M = 5;
  const unsigned long GYRO_SAMPLE_MS = 100;
  const int YAW_LOW_THRESHOLD = 10;
  const int DEFAULT_PITCH_DELTA_THRESHOLD = 40;
  const int DEFAULT_YAW_DELTA_RATE_THRESHOLD = 20;

  int pitchThresh = (TimerSetup.PitchExThresh > 0) ? TimerSetup.PitchExThresh : DEFAULT_PITCH_DELTA_THRESHOLD;
  if (abs(dx) >= pitchThresh)
  {
    pitchSetCounter++;
    pitchClearCounter = 0;
  }
  else
  {
    pitchSetCounter = 0;
    if (abs(dx) < (pitchThresh / 2)) pitchClearCounter++; else pitchClearCounter = 0;
  }

  if (pitchSetCounter >= PITCH_SET_N)
  {
    // set pitch excessive -> stop run
    run_state = false;
  }
  else if (pitchClearCounter >= PITCH_CLEAR_M)
  {
    // clear (no-op for test)
  }

  if (ispeed_state == TAKEOFF || ispeed_state == FLY)
  {
    float yawRate = (float)dz * 1000.0f / (float)GYRO_SAMPLE_MS;
    int yawRateThresh = (TimerSetup.YawRateExThresh > 0) ? TimerSetup.YawRateExThresh : DEFAULT_YAW_DELTA_RATE_THRESHOLD;

    if (abs(yawRate) >= yawRateThresh)
    {
      yawHighCounter++;
      yawLowCounter = 0;
    }
    else if (abs(iangleZ) <= YAW_LOW_THRESHOLD)
    {
      yawLowCounter++;
      yawHighCounter = 0;
    }
    else
    {
      yawHighCounter = 0;
      yawLowCounter = 0;
    }

    if (ispeed_state == TAKEOFF)
    {
      if (yawSlowCount < INT_MAX - 1) yawSlowCount++;
    }

    bool disableFlightChecks = (ispeed_state == FLY && digitalRead(DS3) == LOW);

    if (yawHighCounter >= YAW_HIGH_N)
    {
      if (!disableFlightChecks) { run_state = false; }
    }
    else if (yawLowCounter >= YAW_LOW_M)
    {
      if (!disableFlightChecks) { run_state = false; }
    }
    else if (yawSlowCount >= 50)
    {
      if (!disableFlightChecks) { run_state = false; }
    }
    else
    {
      // nothing
    }
  }
  else
  {
    if (yawSlowCount > 0) yawSlowCount--;
    yawHighCounter = 0;
    yawLowCounter = 0;
  }
}

// Helper: run several neutral cycles to clear internal counters
void clear_counters()
{
  // set state to WAIT so yaw counters reset
  ispeed_state = WAIT;
  for (int i = 0; i < 8; ++i) evaluateHighPitchYaw_local(0,0,0);
}

void test_pitch_hysteresis_sets_runstate_false()
{
  clear_counters();
  run_state = true;
  TimerSetup.PitchExThresh = 0; // use default
  // two consecutive large dx events should trip pitch logic
  evaluateHighPitchYaw_local(50,0,0);
  TEST_ASSERT_TRUE(run_state); // first call shouldn't stop immediately
  evaluateHighPitchYaw_local(50,0,0);
  TEST_ASSERT_FALSE(run_state);
}

void test_yaw_rate_triggers_runstate_false()
{
  clear_counters();
  run_state = true;
  TimerSetup.YawRateExThresh = 0; // use default
  ispeed_state = TAKEOFF;
  // with GYRO_SAMPLE_MS=100, dz=3 -> yawRate=30 deg/sec -> above default 20
  evaluateHighPitchYaw_local(0,0,3);
  TEST_ASSERT_TRUE(run_state);
  evaluateHighPitchYaw_local(0,0,3);
  TEST_ASSERT_FALSE(run_state);
}

int main()
{
  UNITY_BEGIN();
  RUN_TEST(test_pitch_hysteresis_sets_runstate_false);
  RUN_TEST(test_yaw_rate_triggers_runstate_false);
  return UNITY_END();
}
