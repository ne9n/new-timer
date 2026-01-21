#include <unity.h>
#include <cmath>

// Use project param type
#include "cheesehead_timer.h"

// Provide TimerSetup used by the implementation
param TimerSetup;

// Simulate the small Lap accumulation logic from `read_giro()`
static void processLapAccum(float &accumulatedY)
{
  if (fabs(accumulatedY) < 36.0f) return;
  int tenths = (int)(fabs(accumulatedY) / 36.0f);
  TimerSetup.LapCount += (unsigned long)tenths; // units = 0.1 lap
  float consumed = 36.0f * (float)tenths;
  if (accumulatedY > 0) accumulatedY -= consumed; else accumulatedY += consumed;
}

static int signedDelta(int cur, int prev)
{
  int d = cur - prev;
  if (d > 180) d -= 360;
  else if (d < -180) d += 360;
  return d;
}

void test_lap_rollover_positive_direction()
{
  TimerSetup.LapCount = 0;
  float accumulatedY = 0.0f;

  int prev = 350;
  int cur = 10; // wrap: expected signed delta +20
  int d = signedDelta(cur, prev);
  TEST_ASSERT_EQUAL_INT(20, d);
  accumulatedY += (float)d;
  processLapAccum(accumulatedY);
  TEST_ASSERT_EQUAL_UINT64(0, TimerSetup.LapCount);

  // next step: 10 -> 30 (another +20) -> accumulated 40 -> should create 1 tenths
  prev = cur; cur = 30;
  d = signedDelta(cur, prev);
  TEST_ASSERT_EQUAL_INT(20, d);
  accumulatedY += (float)d;
  processLapAccum(accumulatedY);

  TEST_ASSERT_EQUAL_UINT64(1, TimerSetup.LapCount);
  // accumulatedY should have been reduced by 36 -> 40 - 36 = 4
  TEST_ASSERT_TRUE(fabs(accumulatedY - 4.0f) < 0.001f);
}

void test_lap_rollover_negative_direction()
{
  TimerSetup.LapCount = 0;
  float accumulatedY = 0.0f;

  int prev = 10;
  int cur = 350; // reverse wrap: expected signed delta -20
  int d = signedDelta(cur, prev);
  TEST_ASSERT_EQUAL_INT(-20, d);
  accumulatedY += (float)d;
  processLapAccum(accumulatedY);
  TEST_ASSERT_EQUAL_UINT64(0, TimerSetup.LapCount);

  // next: 350 -> 330 => delta -20 -> accumulated -40 -> one tenths negative
  prev = cur; cur = 330;
  d = signedDelta(cur, prev);
  TEST_ASSERT_EQUAL_INT(-20, d);
  accumulatedY += (float)d;
  processLapAccum(accumulatedY);

  TEST_ASSERT_EQUAL_UINT64(1, TimerSetup.LapCount);
  // accumulatedY should be -4 after consuming 36
  TEST_ASSERT_TRUE(fabs(accumulatedY + 4.0f) < 0.001f);
}

int main()
{
  UNITY_BEGIN();
  RUN_TEST(test_lap_rollover_positive_direction);
  RUN_TEST(test_lap_rollover_negative_direction);
  return UNITY_END();
}
