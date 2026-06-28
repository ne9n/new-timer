#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include <Arduino.h>
#include "cheesehead_timer.h"




void crash_check();
void speedState();
bool check_state();
void setSpeedState(speed_state newState);

#endif