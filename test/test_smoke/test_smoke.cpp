#include <unity.h>

void test_smoke()
{
	TEST_ASSERT_TRUE(1);
}

int main()
{
	UNITY_BEGIN();
	RUN_TEST(test_smoke);
	return UNITY_END();
}
