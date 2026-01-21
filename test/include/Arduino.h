// Minimal Arduino stub for host unit tests
#pragma once

#include <cstdint>
using byte = uint8_t;
using boolean = bool;
using unsigned_long = unsigned long;

// Basic test-time time helpers
extern unsigned long test_millis;
inline unsigned long millis() { return test_millis; }
inline void delay(unsigned long ms) { test_millis += ms; }

#define INPUT_PULLUP 2
#define LED_BUILTIN 13
#define HIGH 1
#define LOW 0

inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}

// Minimal digitalRead stub controllable from tests
extern int test_digital_read;
inline int digitalRead(int) { return test_digital_read; }

// Minimal String and Serial stubs used by some modules
#include <string>
#include <algorithm>
#include <cctype>

class String {
	std::string s;
public:
	String() : s() {}
	String(const char *c) : s(c) {}
	void trim() {
		while (!s.empty() && std::isspace((unsigned char)s.back())) s.pop_back();
		while (!s.empty() && std::isspace((unsigned char)s.front())) s.erase(0,1);
	}
	size_t length() const { return s.length(); }
	const char *c_str() const { return s.c_str(); }
	bool equalsIgnoreCase(const char *o) const {
		std::string a = s; std::string b = o; 
		std::transform(a.begin(), a.end(), a.begin(), [](unsigned char c){ return std::tolower(c); });
		std::transform(b.begin(), b.end(), b.begin(), [](unsigned char c){ return std::tolower(c); });
		return a == b;
	}
	bool startsWith(const char *p) const { return s.rfind(p, 0) == 0; }
};

struct SerialClass {
	int available() { return 0; }
	String readStringUntil(char) { return String(""); }
	void println(const char *) {}
	void println(const String &) {}
	void println(int) {}
	void print(const char *) {}
	void print(double) {}
};

extern SerialClass Serial;
