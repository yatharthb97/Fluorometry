 
//Fluorometer with no backward motion of stepper motor

#include <MCP48xx.h>
#include "pins.hpp"
int total_steps;
int no_of_rotations = 0;

const int IR_THRESHOLD = 50;

MCP4822 mcp4822(DAC_CS_PIN);
const int controlvolt = 700;


//DEBUG LEDs
const int BluePIN = 42;
const int GreenPIN = 43;

void forward(int n, unsigned int half_period_ms = 30)
{
  int i =0;
  digitalWrite(EN_PIN, LOW);
  delay(2);
  digitalWrite(DIR_PIN, HIGH);
  delay(2);
  
  while (i < n) 
  {
    digitalWrite(PULSE_PIN, HIGH);
    delay(half_period_ms);
    digitalWrite(PULSE_PIN, LOW);
    delay(half_period_ms);
    i++;
  }
}

void setup() {
  pinMode(BluePIN, OUTPUT);
  pinMode(GreenPIN, OUTPUT);
  digitalWrite(BluePIN, HIGH);
  
   Serial.begin(9600);

  pinMode(PMT_READ_PIN,INPUT);
  
  pinMode(LASER_CNTRL_PIN, OUTPUT);
  pinMode(IR_SENSOR_READ,INPUT); 
  pinMode(PULSE_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

  for (int i = 0; i < 15 ; i++)
  {
    int sensor = analogRead(IR_SENSOR_READ);
    Serial.println(sensor);
    delay(500);
  }
  
  mcp4822.init();
  mcp4822.setGainA(MCP4822:: High);
  mcp4822.setGainB(MCP4822:: Low);
  mcp4822.turnOnChannelA();//turnig on the channel A
  mcp4822.setVoltageA(controlvolt); //500= 0.505 volt controll voltage
  mcp4822.updateDAC(); //updating DAC
  Serial.print("Control Voltage = ");
  Serial.println(controlvolt);
  
  digitalWrite(EN_PIN, HIGH);
  delay(1);
  digitalWrite(DIR_PIN, LOW);
  delay(1);
  digitalWrite(PULSE_PIN, LOW);
  delay(1);
    
  int ii = calib();
  total_steps = ii; //Number of steps per rotation
  
  digitalWrite(BluePIN, LOW);
  digitalWrite(GreenPIN, HIGH);  
}

void loop() 
{
  
    // int time0 = millis();
    
    int cell1, cell2, cell3, cell4;
    int time1;
    double duration;
  
    int xx = total_steps/4 ;
    int integration_time = 4000;
    
    while(true)
    {
        //  Serial.print("xx =  ");
        //  Serial.println(xx);
        //  Serial.println("cell1");
        // delay(4000);
     
        
        //  time1 = millis();
        //  duration = (time1 - time0)/1000 ;
        // Serial.print(duration);
        // Serial.print("\t");
        
        cell1 = read_pmt(integration_time);
        Serial.print(cell1);
        Serial.print("\t");
        forward(xx);
     
        // Serial.println("cell2");
        //delay(4000);
        
        cell2 = read_pmt(integration_time);
        Serial.print(cell2);
        Serial.print("\t");
        forward(xx);

        // Serial.println("cell3");
        // delay(4000);
        
        cell3 =  read_pmt(integration_time);
        Serial.print(cell3);
        Serial.print("\t");
        forward(xx);
        
        // Serial.println("cell4");
        //delay(4000);
        
        cell4 = read_pmt(integration_time);
        Serial.print(cell4);
        Serial.print("\t");
      
        int yy = nforward();
      
        xx = (yy + 3*xx)/4;
        
        Serial.println(xx);
        no_of_rotations++;
    }
}

 
int calib() 
{
  int sensor = analogRead(IR_SENSOR_READ);
  int i =0;
  
  //if(sensor <= IR_THRESHOLD) 
  //  { forward(10, 5); }
  
  while (!sensor)
  {
    forward(1, 5);
    sensor = analogRead(IR_SENSOR_READ);
    //Serial.println(sensor);
    
    if(sensor > IR_THRESHOLD)
      { i++; }
  }
  
  Serial.print("1st step of calibration done: ");
  Serial.println(i);
  
  forward(10, 30);
  i =0;
  
  sensor = analogRead(IR_SENSOR_READ);
  
  while (sensor > IR_THRESHOLD)
  {
    forward(1);
    sensor = digitalRead(IR_SENSOR_READ);
    i++;
  }
  
  Serial.print("2nd step of calibration done: ");
  Serial.println(i+9);
  
  return(i+9);
}
  


int nforward()
{
  int i =0;
  int sensor = analogRead(IR_SENSOR_READ);
  
  while (sensor > IR_THRESHOLD) 
  {
    forward(1);
    sensor = digitalRead(IR_SENSOR_READ);
    i++;
  }
  
  // Serial.println(i);
  forward(2);
  return(i);
}


int read_pmt(unsigned int integn_time_ms)
{
    int signal, noise;
    int i=0;
    double signal_integrated = 0.0;
    double noise_integrated = 0.0;
    double clean_sig_integrated = 0.0;
    
    //Integration
    for(unsigned int i = 0; i < integn_time_ms; i++)
    {
        digitalWrite(LASER_CNTRL_PIN, HIGH);
        delayMicroseconds(500);
        signal = analogRead (PMT_READ_PIN);
        signal_integrated += signal;  
        
        //asm volatile ("dsb"); //Wait for GPIO to synchronise (prevent optimizations)

        digitalWrite(LASER_CNTRL_PIN, LOW);
        delayMicroseconds(500);
        noise = analogRead (PMT_READ_PIN);
        noise_integrated += noise;

        clean_sig_integrated +=  (signal - noise);
    }
    
    signal_integrated = signal_integrated / integn_time_ms;
    noise_integrated = noise_integrated / integn_time_ms;
    clean_sig_integrated = clean_sig_integrated / integn_time_ms;


    //TODO : Clean
    #if FLUORI_DEBUG_MESSAGES == 1
        Serial.print("signal =  ");
        Serial.print(signal_integrated);
        Serial.print("\t");
        Serial.print(noise_integrated);
        Serial.print("\t");
        Serial.println(clean_sig_integrated);
    #endif

    return(clean_sig_integrated);
}
