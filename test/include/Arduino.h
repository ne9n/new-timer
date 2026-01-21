// Minimal Arduino stub for host unit tests
#pragma once

#include <cstdint>
using byte = uint8_t;
using boolean = bool;
using unsigned_long = unsigned long;

extern unsigned long test_millis;
inline unsigned long millis() { return test_millis; }
inline void delay(unsigned long ms) { test_millis += ms; }

#define INPUT_PULLUP 2
#define LED_BUILTIN 13
#define HIGH 1
#define LOW 0

inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
