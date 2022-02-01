
#pragma once
#include <util/atomic.h>

class DCMotor
{
public:

	unsigned int fwdPIN = 99;     //!< Forward motion enable pin
	unsigned int revPIN = 99;	  //!< Reverse motion enable pin
	unsigned int encChAPIN = 99;  //!< Encoder Channel A pin
	unsigned int encChBPIN = 99;  //!< Encoder Channel B pin
	unsigned int encVccPIN = 99;  //!< Encoder Vcc pin


	volatile int pos = 0;   //!< Current position of the motor
	volatile int t_pos = 0; //!< Target position of the motor
	volatile int last_pos = 0; //!< Stores the last macro-position of the motor

	int dir = 1; //!< Direction of motor → {-1, +1}

	volatile int displacement = 0; //!< Stores the required displacement
	
	int min_pos = 0; //!< Minimum valid position for motor
	int max_pos = 0; //!< Maximum valid position for motor


	uint8_t power = 255;							//!< Power of the motor
	const static uint8_t lower_pwr_limit = 150;		//!< Limit for the lowest valid power

	volatile bool alert = false;

	DCMotor(const unsigned int fwdPIN, const unsigned int revPIN) : fwdPIN(fwdPIN), revPIN(revPIN)
	{
		// Set pin modes
		pinMode(fwdPIN, OUTPUT);
		pinMode(revPIN, OUTPUT);


		// Set pins to low
		digitalWrite(fwdPIN, LOW);
		digitalWrite(revPIN, LOW);

	}


	/** @brief Initalizes the encoder resources. */
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

	/** @brief Set safety boundaries for */


	/** @brief Sets the power of the motor to the passed value `new_pwr`.
	 * If `new_pwr` > 255,  */
	void set_pwr(uint8_t new_pwr)
	{
		//Enforcing upper limit
		new_pwr = !(new_pwr > 255)* new_pwr + (new_pwr > 255) * 255;

		//Enforcing lower limit
		power = (new_pwr >= lower_pwr_limit) * new_pwr + 
			   !(new_pwr >= lower_pwr_limit) * lower_pwr_limit;
	}


	/** @brief Sets the direction of the motor.
	 * Negative values → reverse direction.
	 * Positive values & Zero → forward direction. */
	void set_dir(int new_dir)
	{
		dir = (new_dir >= 0) * 1 + !(new_dir >= 0) * -1;
		
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

	/** @brief Returns the pin which is currently driving the motion. */
	void headingPIN()
	{
		return (dir >= 0) * fwdPIN + !(dir >= 0) * revPIN;
	}


	/** @brief Reads the instantaneous position of the motor. */
	void inst_pos() __attribute__((always_inline))
	{
		int inst_pos = 0;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) 
		{
			inst_pos = pos;
		}
		return inst_pos;
	}


	/** @brief Start the motor. Sets the direction pins and the power*/
	void start()
	{
		analogWrite(headingPIN(), power);
	}

	/** @brief Stop the motor by disabling the power. */
	void stop()
	{
		analogWrite(headingPIN(), 0);
	}


	/** @brief Operate the motor in forward direction for `steps` steps. */
	void forward(unsigned int steps)
	{
		last_pos = pos;
		t_pos = pos + steps;
		simple_operate(5);
	}

	/** @brief Operate the motor in reverse direction for `steps` steps. */
	void reverse(unsigned int steps)
	{
		last_pos = pos;
		t_pos = pos - steps;
		simple_operate(5);
	}

	/** @brief Move the motor to `new_pos`. Direction of movement is auto-determined. */
	void move_to(unsigned int new_pos)
	{
		if(new_pos < max_pos & new_pos >= min_pos)
		{
			last_pos = pos;
			t_pos = new_pos;
			simple_operate(5);  
		}	
	}


	/** @brief This is an `interrupt service routine (ISR)` used to update the motor position.
	 * This function is triggered when channel A of the encoder receives a `RISING` signal.
	 * Reads Channel B of encoder and adjusts the position value. */
	void isrChB()
	{
	  int b = digitalRead(encChBPIN);
	  
	  if(b > 0) { pos++; }
	  
	  else { pos--; }
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
			set_speed(speed);

			if (delay_ms > 0)
			{
				delay(delay_ms);
			}
		}
	}

	/** @brief This function operates the motor till it reaches the target position.
	 * This function does not have correction cycles and will only work accurately for very slow speeds.*/
	void simple_operate(delay_ms)
	{
		
		dispalcement = t_pos - pos;
		int instant_pos = inst_pos();

		if(displacement != 0)
		{
			last_pos = pos;
			set_dir(displacement);
			this->start();

			while (instant_pos != t_pos & !alert)
			{
				if (delay_ms > 0)
				{
					delay(delay_ms);
				}
				
				// Read current position
				instant_pos = inst_pos();
				if(instant_pos == last_pos)
				{
					alert = true;
				}
			}

			this->stop();
		}
	}

} // End of class DCMotor