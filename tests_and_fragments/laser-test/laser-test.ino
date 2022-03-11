
int nn;
int rotation = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  int laserpin = 2;
  pinMode(laserpin, OUTPUT); //laser
    
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
 // int ii = calib();
 // nn =ii;
}

void loop() {
  // put your main code here, to run repeatedly:
    int laserpin = 2;
 //  digitalWrite(laserpin, LOW);
   delay(1000);
   digitalWrite(laserpin, HIGH);
   delay(1000);
}
