

#include <MCP48xx.h>

const byte ldacPin=9;
const byte csPin=10;
MCP4822 mcp4822(csPin);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, OUTPUT); //laser
  pinMode(ldacPin,OUTPUT);
  pinMode(A0,INPUT);
  digitalWrite(ldacPin,LOW);
  mcp4822.init();
  mcp4822.setGainA(MCP4822:: High);
  mcp4822.setGainB(MCP4822:: Low);
  mcp4822.turnOnChannelA();//turnig on the channel A
  mcp4822.setVoltageA(243); //500= 0.505 volt controll voltage
  mcp4822.updateDAC(); //updating DAC
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
}


void pmt()
{
  int laserpin =2;
  int pmt1, pmt2;
  int i=0;
  double j=0;
  double k =0.0;
  double diff = 0.0;
  while(i<2000){
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
  j = j/2000;
  k = k/2000;
  diff = diff/2000;
  Serial.print( j);
  Serial.print("\t");
  Serial.print( k);
  Serial.print("\t");
  Serial.print(diff);
  Serial.print("\n");
  Serial.print("\n");
}
