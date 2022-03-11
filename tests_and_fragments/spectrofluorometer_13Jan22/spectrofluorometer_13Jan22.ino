
//Fluorometer with no backward motion of stepper motor
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <MCP48xx.h>

Adafruit_ADS1115 ads1115;
#define ADS1115_RATE_860            0x07;
int nn;
int rotation = 0;
int laserpin = 12;
const byte ldacPin=9;
const byte csPin=53;
MCP4822 mcp4822(csPin);
int controlvolt = 650;


// Serial Events
String received = "";
boolean complete = false;
bool stop_code = true;
// Serial Events
void setup() { Serial.begin(9600);stop_();/*Empty setup*/}

//Seperate callable setup
void setup_() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ldacPin,OUTPUT);
 // pinMode(A0,INPUT);
  pinMode(laserpin, OUTPUT); //laser  
  pinMode(A1,INPUT); //position sensor  
  pinMode(3, OUTPUT); //pulse
  pinMode(4, OUTPUT);  //dir
  pinMode(5, OUTPUT); //enable
  
  digitalWrite(ldacPin,LOW);
  mcp4822.init();
  mcp4822.setGainA(MCP4822:: High);
  mcp4822.setGainB(MCP4822:: Low);
  mcp4822.turnOnChannelA();//turnig on the channel A
  mcp4822.setVoltageA(controlvolt); //500= 0.505 volt controll voltage
  mcp4822.updateDAC(); //updating DAC
  Serial.print("Control Voltage = ");
  Serial.println(controlvolt);

  Serial.println("ADC Range: +/- 5.0V (1 bit = 0.8mV)");
 // Serial.println("Comparator Threshold: 1000 (3.000V)");
  ads1115.begin(); // Initialize ads1015 at the default address 0x48
  ads1115.setGain(GAIN_ONE); 
//  ads1115.setRate(RATE_860);
  int16_t adc0, adc1, adc2, adc3;
  adc0 = ads1115.readADC_SingleEnded(0);
//  adc1 = ads1115.readADC_SingleEnded(1);
//  adc2 = ads1115.readADC_SingleEnded(2);
//  adc3 = ads1115.readADC_SingleEnded(3);
  Serial.print("AIN0: "); Serial.println(adc0);
//  Serial.print("AIN1: "); Serial.println(adc1);
//  Serial.print("AIN2: "); Serial.println(adc2);
//  Serial.print("AIN3: "); Serial.println(adc3);
    
  analogWrite(laserpin, 127);
  digitalWrite(5, HIGH);
  delay(1);
  digitalWrite(4, LOW);
  delay(1);
  digitalWrite(3, LOW);
  delay(1);  
  int ii = calib();
  nn =ii;   //Number of steps per rotation
}

void loop() {
  // put your main code here, to run repeatedly:
 
 if(!stop_code)
 {
     // int time0 = millis();
      double cell1, cell2, cell3, cell4;
      int time1;
      double duration;
      
      // int xx = nn/4 ;
        int xx = 200 ;
       int integration_time = 4000;
      
    //  Serial.print("xx =  ");
    //  Serial.println(xx);
    //  Serial.println("cell1");
      delay(10);
     
    //  Serial.print("cell1");
      cell1 = pmt(integration_time);
    //  time1 = millis();
    //  duration = (time1 - time0)/1000 ;
     // Serial.print(duration);
     // Serial.print("\t");
      Serial.print(cell1);
      Serial.print("\t");
    //  delay(4000000);
      forward(xx);
     // Serial.println("cell2");
      delay(10);
      cell2 = pmt(integration_time);
      Serial.print(cell2);
      Serial.print("\t");
      forward(xx);
    //  Serial.println("cell3");
      delay(10);
      cell3 =  pmt(integration_time);
      Serial.print(cell3);
      Serial.print("\t");
      forward(xx);
    //  Serial.println("cell4");
      delay(10);
      cell4 = pmt(integration_time);
      Serial.print(cell4);
      Serial.print("\t");
      
      int yy = nforward();
      Serial.println(yy);

      xx = (yy + 3*xx)/4;
      
      xx = 200 ;
      rotation++;
    
  } // Stop code scope
}

 
 int calib() {
  int sensor;
  sensor = analogRead(A1);
  int i =0;
  
  if(sensor > 500) {forward(20);}
  
  while (sensor <500 ){
  forward(1);
  sensor = analogRead(A1);
  i++;
  }
  Serial.println("1st step of calibration done");
  Serial.println(i);
  forward(20);
  i =0;
  sensor = analogRead(A1);
  while (sensor <500 ){
  forward(1);
  sensor = analogRead(A1);
  i++;
  }
   Serial.println("2nd step of calibration done");
  Serial.println(i+19);
  return(i+19);
  forward(2);
}
  
 void forward(int n){
 int i =0;
  digitalWrite(5, LOW);// enable Pin
  delay(2);
  digitalWrite(4, HIGH);// dir pin
  delay(2);
  while (i < n) {
  digitalWrite(3, HIGH);// pul pin
 // digitalWrite(laserpin, HIGH);
  delay(20);
  digitalWrite(3, LOW);
  //digitalWrite(laserpin, LOW);
  delay(20);
  i++;
  }}

  int nforward(){
  int i =0;
  int sensor ;
  sensor = analogRead(A1);
  while (sensor < 500) {
  forward(1);
  sensor = analogRead(A1);
  i++;
  }
  
 // Serial.println(i);
  forward(2);
  return(i);
  }


double pmt(int tt){
int16_t pmt1;
  int i=0;
  double j=0;
  pmt1=0;
//  Serial.println("pmtmeasure");
  while(i<tt/8){
 // delay(1);
  pmt1 = ads1115.readADC_SingleEnded(0);
  j = j + pmt1;   
  i++;  
 }
  int uu = tt/8;
  j = j/double(uu);

  return(j);
}



// Serial Events for parsing commands from pySerial - `start` and `stop`.

void start()
{
  setup_(); //Call setup
  stop_code = false; //Enable loop
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
}

void stop_()
{
  stop_code = true; //Stop Loop
  digitalWrite(LED_BUILTIN, LOW);
  
  mcp4822.setVoltageA(00); //controll voltage off
  mcp4822.updateDAC(); //updating DAC
  digitalWrite(5, HIGH); //motor disabled
  digitalWrite(12, LOW); //laser off

  delay(1000);
}


void serialEvent() 
{

  
  while (Serial.available() && !complete) 
  {
    char c = (char)Serial.read();
    received += c;
    if (c == '\n') { complete = true; }
  }

  if(complete)
  {
    if (received.equals("start\n")) { start(); complete=false; received="";}

    else if (received.equals("stop\n"))
    { stop_(); complete=false; received="";}

    else {complete=false; received=""; digitalWrite(A1, HIGH);}
  }
}
