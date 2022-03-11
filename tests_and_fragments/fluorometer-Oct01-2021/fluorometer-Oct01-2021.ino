
//Fluorometer with no backward motion of stepper motor

#include <MCP48xx.h>
int nn;
int rotation = 0;
int laserpin = 2;
const byte ldacPin=9;
const byte csPin=10;
MCP4822 mcp4822(csPin);
int controlvolt = 700;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ldacPin,OUTPUT);
  pinMode(A0,INPUT);
  pinMode(laserpin, OUTPUT); //laser  
  pinMode(A3,INPUT); //position sensor  
  pinMode(5, OUTPUT); //pulse
  pinMode(6, OUTPUT);  //dir
  pinMode(7, OUTPUT); //enable
  
  digitalWrite(ldacPin,LOW);
  mcp4822.init();
  mcp4822.setGainA(MCP4822:: High);
  mcp4822.setGainB(MCP4822:: Low);
  mcp4822.turnOnChannelA();//turnig on the channel A
  mcp4822.setVoltageA(controlvolt); //500= 0.505 volt controll voltage
  mcp4822.updateDAC(); //updating DAC
  Serial.print("Control Voltage = ");
  Serial.println(controlvolt);
  
  digitalWrite(7, HIGH);
  delay(1);
  digitalWrite(6, LOW);
  delay(1);
  digitalWrite(5, LOW);
  delay(1);  
  int ii = calib();
  nn =ii;   //Number of steps per rotation
}

void loop() {
  // put your main code here, to run repeatedly:
 // int time0 = millis();
  int cell1, cell2, cell3, cell4;
  int time1;
  double duration;
  
   int xx = nn/4 ;
   int integration_time = 4000;
  while(1>0){
//  Serial.print("xx =  ");
//  Serial.println(xx);
//  Serial.println("cell1");
 // delay(4000);
 
  
  cell1 = pmt(integration_time);
//  time1 = millis();
//  duration = (time1 - time0)/1000 ;
 // Serial.print(duration);
 // Serial.print("\t");
  Serial.print(cell1);
  Serial.print("\t");
  forward(xx);
 // Serial.println("cell2");
  //delay(4000);
  cell2 = pmt(integration_time);
  Serial.print(cell2);
  Serial.print("\t");
  forward(xx);
//  Serial.println("cell3");
 // delay(4000);
  cell3 =  pmt(integration_time);
  Serial.print(cell3);
  Serial.print("\t");
  forward(xx);
//  Serial.println("cell4");
  //delay(4000);
  cell4 = pmt(integration_time);
  Serial.print(cell4);
  Serial.print("\t");
  
  int yy = nforward();
  

  xx = (yy + 3*xx)/4;
  Serial.println(xx);
  rotation++;
  }
}

 
 int calib() {
  int sensor = analogRead(A3);
  int i =0;
  
  if(sensor > 500) {forward(10);}
  
  while (sensor <500 ){
  forward(1);
  sensor = analogRead(A3);
  if(sensor >500)
   
  i++;
  }
  Serial.println("1st step of calibration done");
  Serial.println(i);
  forward(10);
  i =0;
  sensor = analogRead(A3);
  while (sensor <500 ){
  forward(1);
  sensor = analogRead(A3); 
  i++;
  }
   Serial.println("2nd step of calibration done");
  Serial.println(i+9);
  return(i+9);
  forward(2);
}
  
 void forward(int n){
 int i =0;
  digitalWrite(7, LOW);// enable Pin
  delay(2);
  digitalWrite(6, HIGH);// dir pin
  delay(2);
  while (i < n) {
  digitalWrite(5, HIGH);// pul pin
  delay(30);
  digitalWrite(5, LOW);
  delay(30);
  i++;
  }}

  int nforward(){
  int i =0;
  int sensor = analogRead(A3);
  while (sensor < 500) {
  forward(1);
  sensor = analogRead(A3);
  i++;
  }
  
 // Serial.println(i);
  forward(2);
  return(i);
  }


int pmt(int tt){
int pmt1, pmt2;
  int i=0;
  double j=0;
  double k =0.0;
  double diff = 0.0;
  while(i<tt){
  digitalWrite(laserpin, LOW);
  delayMicroseconds(500);
  pmt1 = analogRead (A0);
  j = j + pmt1;  
    
  digitalWrite(laserpin, HIGH);
  delayMicroseconds(500);
  pmt2 = analogRead (A0);
  k = k + pmt2; 
  diff = diff + pmt2 - pmt1;
  i++;  
 }
  j = j/tt;
  k = k/tt;
  diff = diff/tt;
//  Serial.print("signal =  ");
//  Serial.print( j);
//  Serial.print("\t");
//  Serial.print( k);
//  Serial.print("\t");
//  Serial.println(diff);
  return(diff);
}
