// Minimal Servo stub for unit tests
#pragma once

extern int lastEscWrite;
class Servo {
public:
  void attach(int, int = 1000, int = 2000) {}
  void write(int v) { lastEscWrite = v; }
};
