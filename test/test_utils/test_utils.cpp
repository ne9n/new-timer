#include <Arduino.h>
#include <unity.h>
#include "utils.h"

void test_clamp_below()
{
  TEST_ASSERT_EQUAL_INT(0, clampInt(-5, 0, 10));
}

void test_clamp_within()
{
  TEST_ASSERT_EQUAL_INT(5, clampInt(5, 0, 10));
}

void test_clamp_above()
{
  TEST_ASSERT_EQUAL_INT(10, clampInt(20, 0, 10));
}

void setup()
{
  delay(2000); // allow monitor to attach
  UNITY_BEGIN();
  RUN_TEST(test_clamp_below);
  RUN_TEST(test_clamp_within);
  RUN_TEST(test_clamp_above);
  UNITY_END();
}

void loop() {}
