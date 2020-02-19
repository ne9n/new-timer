void ledUpdate()
{
  unsigned long currentMillis1 = millis();

  switch (led_state)
  {
    case 1:
      {
        digitalWrite(LED_BUILTIN, HIGH);
        led_state = 2;
        break;
      }
    case 2:
      {
        digitalWrite(LED_BUILTIN, LOW);
        led_state = 1;
        break;

      }
  }
}
