 int calib()
 {
  
  int sensor = analogRead(IR_SENSOR_READ_PIN);
  int i = 0;
  
  if(sensor > 500) 
    {forward(10);}
  
  while (sensor < 500)
  {
      forward(1);
      sensor = analogRead(IR_SENSOR_READ_PIN);
      
      #if FLUORI_DEBUG_MESSAGES == 1
        Serial.println(sensor);
      #endif

      if(sensor > 500)  
        { i++; } 
  }

  Serial.println("1st step of calibration done");
  Serial.println(i);
  
  forward(10); //OFFSET by 10 steps from START
  i =0;

  sensor = analogRead(IR_SENSOR_READ_PIN);

  //Counting Steps from the start
  while (sensor <500 )
  {
      forward(1);
      sensor = analogRead(IR_SENSOR_READ_PIN);
      i++;
  }

  Serial.println("2nd step of calibration done");
  Serial.println(i+9); //Compensating for the 11 step offset before (10+1)
  return(i+9);
  
  }



//! very much like forward/Calibrate
int nforward()
{
    int i =0;
    int sensor = analogRead(IR_SENSOR_READ_PIN);
    while (sensor < 500) {
    forward(1);
    sensor = analogRead(IR_SENSOR_READ_PIN);
    i++;
    }
    
    // Serial.println(i);
    forward(2);
    return(i);
}