
#pragma once
#include <util/atomic.h>



class DCMotor
{
public:

	unsigned int fwdPIN = 999;     //!< Forward motion enable pin
	unsigned int revPIN = 999;	  //!< Reverse motion enable pin
	unsigned int encChAPIN = 999;  //!< Encoder Channel A pin
	unsigned int encChBPIN = 999;  //!< Encoder Channel B pin
	unsigned int encVccPIN = 999;  //!< Encoder Vcc pin


	volatile int pos = 0;   //!< Current position of the motor
	int t_pos = 0; //!< Target position of the motor
	int last_pos = 0; //!< Stores the last macro-position of the motor
	int dir = 1; //!< Direction of motor → {-1, +1}
	uint8_t power = 255;  						//!< Power of the motor

	volatile int displacement = 0; //!< Stores the required displacement
	int min_pos = 0; //!< Minimum valid position for motor
	int max_pos = 0; //!< Maximum valid position for motor
	uint8_t lower_pwr_limit = 150;		//!< Limit for the lowest valid power

	volatile bool alert_flag = false; //!< It is set when the motor is unable to move after being driven. 
	unsigned int alert_pin = 999;			//!< Pin that is set when alert is triggered.


	/** @brief Constructor that initilizes the forward and reverse pins. */
	DCMotor(const unsigned int fwdPIN, const unsigned int revPIN) : fwdPIN(fwdPIN), revPIN(revPIN)
	{
		// Set pin modes
		pinMode(fwdPIN, OUTPUT);
		pinMode(revPIN, OUTPUT);

		// Set pins to low
		digitalWrite(fwdPIN, LOW);
		digitalWrite(revPIN, LOW);

	}


	/** @brief Initalizes the encoder pins. */
	void init_encoder(const unsigned int encChAPIN, const unsigned int encChBPIN, const unsigned int encVCCPIN)
	{
		this->encChAPIN = encChAPIN;
		this->encChBPIN = encChBPIN;
		this->encVccPIN = encVCCPIN;

		pinMode(encVccPIN, OUTPUT);
		digitalWrite(encVccPIN, HIGH);

		pinMode(encChAPIN, INPUT);
		pinMode(encChBPIN, INPUT);

		attachInterrupt(attachInterrupt(encChAPIN), isrChB, RISING);

	}

	/** @brief Initalizes the `alert_pin` and enables alert feature. */
	void init_alert(const unsigned int alert_pin)
	{
		this->alert_pin = alert_pin;
		pinMode(alert_pin, OUTPUT);
	}


	/** @brief Sets the current position as position zero. 
	 * It also resets the target position to zero.*/
	void set_zero_pos()
	{
		noInterrups();
			pos = 0;
		interrupts();
		t_pos = 0;
	}	


// Common Control Functions

	/** @brief Sets the power of the motor to the passed value `new_pwr`.
	 * The power is bounded between [`low_pwr_limit`, 255]. */
	void set_pwr(uint8_t new_pwr)
	{
		//Enforcing upper limit
		new_pwr = !(new_pwr > 255)* new_pwr + (new_pwr > 255) * 255;

		//Enforcing lower limit
		power = (new_pwr >= lower_pwr_limit) * new_pwr + 
			   !(new_pwr >= lower_pwr_limit) * lower_pwr_limit;
	}

	/** @brief Start the motor. Sets the direction pins and the power. */
	void start()
	{
		analogWrite(headingPIN(), power);
	}

	/** @brief Stop the motor by disabling the power. */
	void stop()
	{
		analogWrite(headingPIN(), 0);
	}


	/** @brief Drive the motor for `drive_time_ms` milliseconds. */
	void drive_ms(unsigned int drive_time_ms)
	{
		start();
		delay(drive_time_ms);
		stop();
	}


// Step Counting 

	/** @ Update the target position of the motor. */
	void translate(const int steps)
	{
		t_pos = t_pos + steps;
	}


	/** @brief Operate the motor in forward direction for `steps` steps. */
	void forward(unsigned int steps)
	{
		last_pos = pos;
		t_pos = t_pos + steps;
		simple_operate(5);
	}

	/** @brief Operate the motor in reverse direction for `steps` steps. */
	void reverse(unsigned int steps)
	{
		last_pos = pos;
		t_pos = pos - steps;
		operate(5);
	}

	/** @brief Move the motor to `new_pos`. Direction of movement is auto-determined. */
	void move_to(unsigned int new_pos)
	{
		last_pos = pos;
		t_pos = new_pos;
		operate(5);
	}


	/** @brief Operates the dc motor with a fixed number of `correction cycles (corr_cycles)`.
	 * This operation function corrects errors due to motor run-off. */
	void operate(unsigned int corr_cycles, unsigned int delay_ms=0)
	{
		for(unsigned int i = 0; i < depth; i++)
		{
			// set target position
			//int target = 1200;
			int target = 250 * sin(prevT/1e6);

			// PID constants
			float kp = 1;
			float kd = 0.025;
			float ki = 0.0;

			// time difference
			long currT = micros();
			float deltaT = ((float) (currT - prevT))/( 1.0e6 );
			prevT = currT;

			// Read the position in an atomic block to avoid a potential
			// misread if the interrupt coincides with this code running
			// see: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
			int pos = 0;
			noInterrups();
			  pos = posi;
			interrupts()
			
			// error
			int e = pos - target;

			// derivative
			float dedt = (e-eprev)/(deltaT);

			// integral
			eintegral = eintegral + e*deltaT;

			// control signal
			float u = kp*e + kd*dedt + ki*eintegral;

			// motor power
			float speed = fabs(u) * (speed <= 255) + fabs(u) * !(speed <= 255) * 255;


			// motor direction
			int dir = (u < 0) * -1 + !(u < 0) * 1;


			set_direction(dir);
			set_power(speed);

			if (delay_ms > 0)
			{
				delay(delay_ms);
			}
		}
	}

	/** @brief This function operates the motor till it reaches the target position.
	 * This function does not have correction cycles and will only work accurately 
	 * for very slow speeds.*/
	void operate(unsigned int drive_delay_ms = 0)
	{

		int dispalcement = t_pos - pos;
		int instant_pos = inst_pos();

		if(displacement != 0)
		{
			last_pos = pos;
			set_dir(displacement);
			
			start();

			while(instant_pos != t_pos && !alert_flag)
			{
				if(drive_delay_ms != 0)
				{
					delay(drive_delay_ms);
				}
				
				// Read current position
				instant_pos = inst_pos();
				
				if(instant_pos == last_pos)
				{
					//alert(true);
				}
			}

			stop();
		}
	}


	/** @brief Function that is called in the `loop()` 
	 * function that will continuesly operate the motor.*/
	void drive()
	{

	}

private:

	/** @brief This is an `interrupt service routine (ISR)` used to update the motor position.
	 * This function is triggered when channel A of the encoder receives a `RISING` signal.
	 * Reads Channel B of encoder and adjusts the position value. */
	void isrChB()
	{
	  int b = digitalRead(encChBPIN);
	  if(b > 0) { pos++; }
	  else { pos--; }
	}


	/** @brief Manages the alert functionality where 
	 * the motor is stuck and unable to drive.
	 * Only works when the `alert_pin` is set. */
	void alert(bool status)
	{
		if (alert_pin != DC_MOTOR_INVALID_PIN)
		{
			alert_flag = status;
			digitalWrite(alert_pin, status);
		}	
	}


	/** @brief Returns the pin which is currently driving the motion. */
	void headingPIN()
	{
		return (dir >= 0) * fwdPIN + !(dir >= 0) * revPIN;
	}



	/** @brief Reads the instantaneous position of the motor. */
	void inline inst_pos()
	{
		int inst_pos = 0;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			inst_pos = pos;
		}
		return inst_pos;
	}


	/** @brief Sets the direction of the motor.
	 * Negative values → reverse direction.
	 * Positive values & Zero → forward direction. */
	void set_dir(int new_dir)
	{
		this->dir = (new_dir >= 0) * 1 + !(new_dir >= 0) * -1;
		
		if(dir == 1)
		{
			pinMode(fwdPIN, HIGH);
			pinMode(revPIN, LOW);
		}

		if(dir == -1)
		{
			pinMode(fwdPIN, LOW);
			pinMode(revPIN, HIGH);
		}
		
	}

} // End of class DCMotor



int target = 250 * sin(prev_time/1e6);

// PID constants
const float pid_kp = 1;   	//!< Proportional constant (Corrects the current deviation)
const float pid_kd = 0.025;	//!< Derivative constant (corrects "changing" errors)
const float pid_ki = 0.0;   //!< Integral Constant (corrects accumulated error)

static long curr_time = micros();
float delta_time = ((float) (curr_time - prev_time))/( 1.0e6 );
static long prev_time = curr_time;

// Read the position in an atomic block to avoid a potential
// misread if the interrupt coincides with this code running
// see: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
int pos = 0; 
ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
  pos = posi;
}

posi = inst_pos();
int e = posi - target; //error
float dedt = (e - eprev)/(delta_time); // derivative 
eintegral = eintegral + e * delta_time; // integral

//control power
float pwr = kp * e + 
			kd * dedt + 
			ki * eintegral;
float pwr = fabs(pwr);
if(pwr > 255) { power = 255; }


//set direction and update power of motor
set_dir(pwr);
start();

eprev = e; //store previous error


// Other file ↓
/////////////////////////////////////////////////////////////////////

#include <util/atomic.h> // For the ATOMIC_BLOCK macro

#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE
#define PWM 5
#define IN2 6
#define IN1 7

volatile int posi = 0; // specify posi as volatile: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
long prevT = 0;
float eprev = 0;
float eintegral = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ENCA,INPUT);
  pinMode(ENCB,INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA),readEncoder,RISING);
  
  pinMode(PWM,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  
  Serial.println("target pos");
}
 
void loop() {

  // set target position
  //int target = 1200;
  int target = 250*sin(prevT/1e6);

  // PID constants
  float kp = 1;
  float kd = 0.025;
  float ki = 0.0;

  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  // Read the position in an atomic block to avoid a potential
  // misread if the interrupt coincides with this code running
  // see: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
  int pos = 0; 
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    pos = posi;
  }
  
  // error
  int e = pos - target;

  // derivative
  float dedt = (e-eprev)/(deltaT);

  // integral
  eintegral = eintegral + e*deltaT;

  // control signal
  float u = kp*e + kd*dedt + ki*eintegral;

  // motor power
  float pwr = fabs(u);
  if( pwr > 255 ){
    pwr = 255;
  }

  // motor direction
  int dir = 1;
  if(u<0){
    dir = -1;
  }

  // signal the motor
  setMotor(dir,pwr,PWM,IN1,IN2);


  // store previous error
  eprev = e;

  Serial.print(target);
  Serial.print(" ");
  Serial.print(pos);
  Serial.println();
}


//---->>


void setMotor(int dir, int pwmVal, int pwm, int in1, int in2){
  analogWrite(pwm,pwmVal);
  if(dir == 1){
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
  }
  else if(dir == -1){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
  }
  else{
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
  }  
}

void readChB()
{
  int b = digitalRead(encChBPIN);
  if(b > 0)
  {
    posi++;
  }
  else
  {
    posi--;
  }
}