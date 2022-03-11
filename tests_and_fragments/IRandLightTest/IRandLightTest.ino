const int LED_PIN = 12;
  const int IR_READ_PIN = A0; 

void setup() {
  // put your setup code here, to run once:
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  digitalWrite(LED_PIN, LOW);
}
