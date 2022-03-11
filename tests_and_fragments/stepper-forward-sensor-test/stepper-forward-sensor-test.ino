int nn;
int rotation = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A3,INPUT); //position sensor  
  pinMode(5, OUTPUT); //pulse
  pinMode(6, OUTPUT);  //dir
  pinMode(7, OUTPUT); //enable
  digitalWrite(7, HIGH);
  delay(1);
  digitalWrite(6, LOW);
  delay(1);
  digitalWrite(5, LOW);
  delay(1);  
  int ii = calib();
  nn =ii;
}
  

void loop() {
  // put your main code here, to run repeatedly:
  int xx = nn/4 ;
  while(1>0){
  Serial.print("xx =  ");
  Serial.println(xx);
  Serial.println("cell1");
  delay(4000);
  
  forward(xx);
  Serial.println("cell2");
  delay(4000);
  
  forward(xx);
  Serial.println("cell3");
  delay(4000);

  forward(xx);
  Serial.println("cell4");
  delay(4000);
  
  int yy = nforward();
  

  xx = (yy + 3*xx)/4;
  
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
  while (sensor <500 )
  {
    forward(1);
    sensor = analogRead(A3); 
    i++;
  }
    Serial.println("2nd step of calibration done");
    Serial.println(i+9);
    return(i+9);
    //forward(2); //DEAD CODE
}
  
 void forward(int n)
 {
 int i =0;
  digitalWrite(7, LOW);// enable Pin
  delay(2);
  digitalWrite(6, HIGH);// dir pin
  delay(2);
  
  while (i < n) 
  {
    digitalWrite(5, HIGH);// pul pin
    delay(30);
    digitalWrite(5, LOW);
    delay(30);
    i++;
  }
}

void nforward()
{
  int i =0;
  int sensor = analogRead(A3);
  while (sensor < 500) {
  forward(1);
  sensor = analogRead(A3);
  i++;
}
  
  Serial.println(i);
  forward(2);
  return(i);
  }
