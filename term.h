#ifndef TERM_H
#define TERM_H
//#include <ARDUINO.H>


void banner();
void saveData();
void get1Input();
void menuValues ();
void read_giro();
void plotDebug();
void term_ctrl();
// Non-blocking telemetry update; call from main `loop()`
void telemetryUpdate();
// telemetry mode flag (true = printing telemetry each loop)
extern bool telemetryMode;



#endif