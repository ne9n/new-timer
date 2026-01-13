#include <Arduino.h>
#include <OneButton.h>
#include "cheesehead_timer.h"

class Button {
  
  private:
    byte pin;
    byte state;
    byte lastReading;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 50;
    
  public:
    Button(byte pin);
    void init();
    void update();
    byte getState();
    bool isPressed();
};
const int buttonPin = BUTTONPIN;
OneButton button(buttonPin);

enum State { STOPPED, STARTING, RUN }; // Added RUN state


State currentState = STOPPED;


extern bool run_state;

// Variable to track if a press-and-hold is in progress
bool pressAndHoldInProgress = false;



void handleTap() {
  Serial.println("button tap \n");
  if (!pressAndHoldInProgress) { // Only handle taps if not in a press-and-hold
    switch (currentState) {
      case STOPPED:
       currentState = STARTING;       
        break;
      case RUN:
         currentState = STOPPED;   
        break;
    }
  }
}

void handleDoubleClick() {
   Serial.print("double\n");
 if (!pressAndHoldInProgress){ // Only handle double clicks if not in a press-and-hold
  
}}

void handleLongPressStart() {
  Serial.println("longpress\n");
  pressAndHoldInProgress = true;  // Flag that we're in a press-and-hold
}

void handleLongPressStop() {
  if (pressAndHoldInProgress && currentState == STOPPED) { // Switch to RUN only from OPERATIONAL
    currentState = RUN;
    Serial.println("Entering RUN State");
  }
  pressAndHoldInProgress = false; // Reset the flag
}


void handleLongPress() 
{  // This function is no longer used for state change

}


void executeOperationalSubstate() 
{
  
}

void ButtonSetup()
{
  Serial.print("ButtonSetup\n");
  button.attachClick(handleTap);
  button.attachDoubleClick(handleDoubleClick);
  button.attachLongPressStart(handleLongPressStart); // Start tracking long press
  button.attachLongPressStop(handleLongPressStop);  // Track when long press ends
}

void updateButton() 
{
  button.tick();
/**
 Serial.print("buttton state =   ");
 Serial.print(currentState);
 Serial.print(" button \n ");*/
  switch (currentState) {
    case STOPPED:
      // reset and stop state machine 
      run_state = false;
      break;
    case STARTING:
      run_state = true; 
      currentState = RUN;
      break;
    case RUN:
     // init and start state machine 
// need assses to speed state variables 
   
      break;
  }
}
