

const int IN_PIN = 12;
const int VCC_PIN = 13;

void setup()
{
	pinMode(IN_PIN, OUTPUT);
	pinMode(VCC_PIN, OUTPUT);

	digitalWrite(VCC_PIN, HIGH);

}


void loop()
{
	digitalWrite(IN_PIN, HIGH);
	delay(500);

	digitalWrite(IN_PIN, LOW);
	delay(500);
}
