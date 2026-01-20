#include <Arduino.h>
#include <unity.h>

void test_smoke_true()
{
    TEST_ASSERT_EQUAL(1, 1);
}

void setup()
{
    delay(2000); // allow serial monitor to attach if needed
    UNITY_BEGIN();
    RUN_TEST(test_smoke_true);
    UNITY_END();
}

void loop() {}
