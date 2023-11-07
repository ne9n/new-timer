
int debounce()
{
  // Variables will change:
  static int buttonState = LOW;             // the current reading from the input pin
  static int lastButtonState = LOW;   // the previous reading from the input pin

  static unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
  static unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTONPIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState)
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState)
    {
      buttonState = reading;

    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  return !buttonState;
}
