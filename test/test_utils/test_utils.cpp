#include <unity.h>
#include "utils.h"

void test_clamp_within()
{
	TEST_ASSERT_EQUAL_INT(5, clampInt(5, 0, 10));
}

void test_clamp_low()
{
	TEST_ASSERT_EQUAL_INT(0, clampInt(-7, 0, 10));
}

void test_clamp_high()
{
	TEST_ASSERT_EQUAL_INT(10, clampInt(123, 0, 10));
}

void test_signed_delta_simple()
{
	TEST_ASSERT_EQUAL_INT(10, signedAngleDelta(10, 0));
	TEST_ASSERT_EQUAL_INT(-10, signedAngleDelta(0, 10));
}

void test_signed_delta_wrap()
{
	// 359 - 9 = 350 -> should become -10
	TEST_ASSERT_EQUAL_INT(-10, signedAngleDelta(359, 9));
	// -179 to 179 check
	TEST_ASSERT_EQUAL_INT(2, signedAngleDelta(181, 179));
	TEST_ASSERT_EQUAL_INT(-2, signedAngleDelta(179, 181));
}

int main()
{
	UNITY_BEGIN();
	RUN_TEST(test_clamp_within);
	RUN_TEST(test_clamp_low);
	RUN_TEST(test_clamp_high);
	RUN_TEST(test_signed_delta_simple);
	RUN_TEST(test_signed_delta_wrap);
	return UNITY_END();
}
