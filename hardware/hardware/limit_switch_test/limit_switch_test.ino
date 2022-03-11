

  unsigned int time_assert_ms = 0;    //!< Store of the last trigger time for debouncing.
  bool assert_limit_on = HIGH;      //!< Assert limit on either {HIGH or LOW}
  unsigned int debounce_time_ms = 100;
  volatile bool limit_asserted = false;   //!< Indicates that limit was asserted

  
  void isr_changedetected()
  { 
    if(millis() - time_assert_ms < debounce_time_ms)
    {
      limit_asserted = (digitalRead(9) == assert_limit_on);
      Serial.println("Hit!");
      if(!limit_asserted)
      {
        limit_asserted = true;
        //extern_flag = volatile(true);
        time_assert_ms = millis();
      }

      else
      {
        limit_asserted = false;
        //extern_flag = volatile(false);
      } 
    }
  }


void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A2, INPUT);
  pinMode(A0, INPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  //attachInterrupt(digitalPinToInterrupt(10), isr_changedetected, HIGH);
}

void loop()
{
  //digitalWrite (LED_BUILTIN, limit_asserted);
  unsigned int x = analogRead(A2);
  unsigned int y = analogRead(A0);
  Serial.print(x == 1023);
  Serial.print('\t');
  Serial.println(y == 1023);
  delay(200);
}
