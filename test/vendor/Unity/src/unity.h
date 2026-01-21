/* vendored unity.h - minimal for tests */
#ifndef UNITY_FRAMEWORK_H
#define UNITY_FRAMEWORK_H

#include <stdint.h>
#include <stdio.h>

typedef int UNITY_INT;
typedef unsigned int UNITY_UINT;
typedef unsigned short UNITY_LINE_TYPE;

struct UNITY_STORAGE_T {
    const char* TestFile;
    UNITY_UINT NumberOfTests;
    UNITY_UINT TestFailures;
    UNITY_UINT TestIgnores;
    int CurrentTestFailed;
    int CurrentTestIgnored;
};

extern struct UNITY_STORAGE_T Unity;

void UnityBegin(const char* name);
int UnityEnd(void);

typedef void (*UnityTestFunction)(void);

#define UNITY_BEGIN() UnityBegin(__FILE__)
#define UNITY_END() UnityEnd()
#define RUN_TEST(func) UnityDefaultTestRun(func, #func, __LINE__)

void UnityDefaultTestRun(UnityTestFunction Func, const char* FuncName, const int FuncLine);

#define TEST_ASSERT_TRUE(x) if (!(x)) { Unity.CurrentTestFailed = 1; }
#define TEST_ASSERT_FALSE(x) if (x) { Unity.CurrentTestFailed = 1; }
#define TEST_ASSERT_EQUAL_INT(a,b) if ((a)!=(b)) { Unity.CurrentTestFailed = 1; }
#define TEST_ASSERT_EQUAL_UINT64(a,b) if ((a)!=(b)) { Unity.CurrentTestFailed = 1; }
#define TEST_ASSERT_TRUE_MESSAGE(x,m) if (!(x)) { Unity.CurrentTestFailed = 1; }

#endif
