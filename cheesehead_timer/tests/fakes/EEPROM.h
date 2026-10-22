#ifndef TESTS_FAKES_EEPROM_H
#define TESTS_FAKES_EEPROM_H

class FakeEEPROM
{
public:
  template <typename T>
  void put(int, const T &) {}

  bool commit() { return true; }
};

extern FakeEEPROM EEPROM;

#endif
