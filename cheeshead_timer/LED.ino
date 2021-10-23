void ledUpdate()
{
  
  unsigned long currentMillis1 = millis();

  switch (led_state)
  {
    case 1:
      {
        digitalWrite(LED3,LOW);
        digitalWrite(LED4,LOW);
        digitalWrite(LED5,LOW);
        led_state = 2;
        break;
      }
    case 2:
      {
          digitalWrite(LED3,HIGH);
          digitalWrite(LED4,LOW);
          digitalWrite(LED5,LOW);

        led_state = 1;
        break;

      }
  }
}

void ledTest()
{
    for (i=0; i<500; i++)
  {
    
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
    digitalWrite(LED5, HIGH);

    delay(5000);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);

    delay(5000);
  }
}

void led_fast()
{
  digitalWrite(LED3, LOW);
  delay (1000);
  digitalWrite(LED3, HIGH);
  delay (1000);
}

void led_SLOW()
{
  digitalWrite(LED3, LOW);
  delay (5000);
  digitalWrite(LED3, HIGH);
  delay (5000);
}
