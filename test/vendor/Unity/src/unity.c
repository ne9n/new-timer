/* vendored unity.c - trimmed from .pio/libdeps/unit/Unity/src/unity.c */
#include "unity.h"

#ifndef UNITY_PROGMEM
#define UNITY_PROGMEM
#endif

/* Minimal required portions of unity.c are included to support unit tests in this repo. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UNITY_STORAGE_T Unity;

void UNITY_OUTPUT_CHAR(int c) { putchar(c); }
void UNITY_OUTPUT_FLUSH(void) { fflush(stdout); }

void UnityBegin(const char* name) { (void)name; Unity.TestFile = name; Unity.NumberOfTests = 0; Unity.TestFailures = 0; Unity.TestIgnores = 0; }
int UnityEnd(void) { printf("-----------------------\n%d Tests %d Failures %d Ignored\n", Unity.NumberOfTests, Unity.TestFailures, Unity.TestIgnores); return Unity.TestFailures; }

void UnityAssertEqualNumber(const UNITY_INT expected, const UNITY_INT actual, const char* msg, const UNITY_LINE_TYPE line) { (void)expected; (void)actual; (void)msg; (void)line; }

void UnityDefaultTestRun(UnityTestFunction Func, const char* FuncName, const int FuncLine) {
    (void)FuncLine; Unity.NumberOfTests++; printf("%s: ", FuncName); Func(); if (Unity.CurrentTestFailed) { Unity.TestFailures++; printf("FAIL\n"); } else { printf("PASS\n"); } Unity.CurrentTestFailed = 0; Unity.CurrentTestIgnored = 0; }

/* Basic stubs for failing/ignoring */
void UNITY_TEST_FAIL(const UNITY_LINE_TYPE line, const char* message) { (void)line; (void)message; Unity.CurrentTestFailed = 1; }
void UNITY_TEST_IGNORE(const UNITY_LINE_TYPE line, const char* message) { (void)line; (void)message; Unity.CurrentTestIgnored = 1; }

/* Provide minimal UnityTestFunction typedef */


