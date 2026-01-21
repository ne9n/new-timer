GXX = g++
INCLUDES = -Itest/include -I.
CXXFLAGS = -std=c++11 -g $(INCLUDES)
UNITY = test/vendor/Unity/src/unity.c

TESTS = test_utils test_state_machine test_gyro test_lap_accum test_smoke

all: test

bin:
	@mkdir -p bin

test: bin $(TESTS:%=bin/%.exe)

bin/%.exe: test/%/%.cpp $(UNITY) test/test_support.c | bin
	$(GXX) $(CXXFLAGS) $< $(UNITY) test/test_support.c -o $@

bin/test_lap_accum.exe: test/test_gyro/test_lap_accum.cpp $(UNITY) test/test_support.c | bin
	$(GXX) $(CXXFLAGS) $< $(UNITY) test/test_support.c -o $@

clean:
	rm -rf bin

.PHONY: all test clean bin
