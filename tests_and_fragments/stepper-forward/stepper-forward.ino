
//Pins
const int EN_PIN = 5;
const int DIR_PIN = 4;
const int PULSE_PIN = 3;

const int PMT_PIN = A0;
const int IR_SENSOR_READ_PIN = A1;



void setup() {
  pinMode(EN_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PULSE_PIN, OUTPUT);
  pinMode(IR_SENSOR_READ_PIN, INPUT);
  digitalWrite(PULSE_PIN, HIGH);
  //delay(5000);
  Serial.begin(9600);
 
  digitalWrite(EN_PIN, HIGH);
  delay(1);
  digitalWrite(DIR_PIN, LOW);
  delay(1);
  digitalWrite(PULSE_PIN, LOW);
  delay(1);

  //align();
  delay(2000);
}

void loop() {
  int nn;
  nn=200;
  forward (4*nn);
  delay(2000);
  int sensorval;
  sensorval = analogRead(IR_SENSOR_READ_PIN);
  Serial.println(sensorval);
// delay(500);
}


void forward(int n)
{
  digitalWrite(EN_PIN, LOW);// enable Pin
  delay(2);
  digitalWrite(DIR_PIN, HIGH);// dir pin
  delay(2);

  int i =0;
  while (i < n) 
  {
    //digitalWrite(EN_PIN, LOW);
    digitalWrite(PULSE_PIN, HIGH);// pul pin
    delay(3);
    digitalWrite(PULSE_PIN, LOW);
    //digitalWrite(EN_PIN, HIGH);
    delay(3);
    i++;
  }
}


/** @brief Aligns the sensor to the START position, by exploring the rotation space. */
void align()
{
    //double step_angle = STEPPER_RES_DEGREE;
    //int steps = floor(360.0/step_angle);
    
    unsigned int steps = 200;
    unsigned int sensor_val[steps] = {1024};

    for(unsigned int i = 0; i < steps; i++)
    {
        sensor_val[i] = analogRead(IR_SENSOR_READ_PIN);
        delay(15);
        forward(1); //Forward one step
    }

    //Find Minimum & Maximum
    unsigned int max_val, min_val = sensor_val[0];
    unsigned int min_index, max_index = 0;
    for(unsigned int i = 0; i < steps; i++)
    {
        if(sensor_val[i] < min_val)
        {
            min_val = sensor_val[i];
            min_index = i;
        }

        if(sensor_val[i] > max_val)
        {
            max_val = sensor_val[i];
            max_index = i;
        }
    }

    //Move Motor to desired poisiton
    unsigned int align_pos = min_index;
    unsigned int reverse_steps = steps - align_pos;
    delay(2000);
    forward(200 - reverse_steps + 6);

    bool aligned_status = (analogRead(IR_SENSOR_READ_PIN) < 50);

    if(aligned_status == false)
    {
        Serial.println("IRSensor : Alignment failed !");
    }

}
