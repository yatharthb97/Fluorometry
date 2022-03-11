

#include <MCP48xx.h>
#include <ADS1X15.h>
//#include "Adafruit_ADS1015.h"

const byte csPin=10;
MCP4822 mcp4822(csPin);

#define ADS_GND 0x48
ADS1115 ADS(0x4A);
//Adafruit_ADS1115 ads;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Serial is set");
  mcp4822.init();
  mcp4822.setGainA(MCP4822:: High);
  mcp4822.setGainB(MCP4822:: Low);
  mcp4822.turnOnChannelA();//turnig on the channel A
  mcp4822.setVoltageA(243); //500= 0.505 volt controll voltage
  mcp4822.updateDAC(); //updating DAC
  Serial.println("MCP is set");
// ADC -> PMT Read Code ↓
    
  ADS.begin();
  ADS.setGain(0);      // 6.144 volt
  ADS.setDataRate(7);  // fast
  ADS.setMode(0);      // continuous mode
  ADS.readADC(0);      // first read to trigger
    
  //adc.begin(0x49);
  //Initialize the ADC for operation using specified address of 0x49.
  Serial.println("ADC is set");

// Test Resource
analogWrite(A3, 125);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("-> ");
  Serial.println(ADS.getValue());
  delay(500);
   //adc0 = ads.readADC_SingleEnded(0);
   //Serial.println(adc0);
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
