#include <MCP48xx.h>

const byte ldacPin=9;
const byte csPin=10;
MCP4822 mcp4822(csPin);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ldacPin,OUTPUT);
  digitalWrite(ldacPin,LOW);
  mcp4822.init();
  mcp4822.setGainA(MCP4822:: High);
  mcp4822.setGainB(MCP4822:: Low);
  mcp4822.turnOnChannelA();//turnig on the channel A
  mcp4822.setVoltageA(00); //controll voltage off
  mcp4822.updateDAC(); //updating DAC
  

  pinMode(A3,INPUT); //position sensor  
  pinMode(2, OUTPUT); //laser
  pinMode(5, OUTPUT); //pulse
  pinMode(6, OUTPUT);  //dir
  pinMode(7, OUTPUT); //enable
  digitalWrite(7, HIGH); //motor disabled
  delay(1);
  digitalWrite(6, LOW);
  delay(1);
  digitalWrite(5, HIGH);
  delay(1);
  digitalWrite(2, LOW); //laser off
}

void loop() {
  
   // put your main code here, to run repeatedly:
int   pmt = analogRead(A0);
   Serial.println(pmt);
   delay(1000);
}
