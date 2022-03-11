// Encoder

	void isrChB()
	{
		unsigned int b = digitalRead(ENCODER_CHB_PIN);
		if(b == HIGH) { pos++; }
		else { pos--; }
	}


// Calibration

	void calibrate()
	{
		operate_motor = true;
		//motor.reverse(100);
		attachInterrupt(digitalPinToInterrupt(IR_SENSOR1_READ_PIN), isrCalibrate, RISING);

		tpos -= 1000;
		while(!calibrated)
		{
			operate();
		}

		// Zero Position is set
		pos = 0;
		tpos = 0;
		last_pos = 0;

	}

	void isrCalibrate()
	{
		calibrated = true;
		operate_motor = false;
	}




// Operate
	inline void operate()
	{
		if(operate_motor)
		{
			if(tpos != inst_pos())
			{
				set_motor(); 
			}
		}	
	}


	void set_motor()
	{
		displacement = tpos - inst_pos();
		dir = (displacement >= 0)*1 + (displacement<0)*-1;

		if(dir > 0)
		{
			analogWrite(FWD_PIN, power);
		}


		if(dir < 0)
		{
			analogWrite(REV_PIN, power);
		}
	}



	/** @brief Reads the instantaneous position of the motor. */
	inline long int inst_pos()
	{
		long int inst_pos = 0;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			inst_pos = pos;
		}
		return inst_pos;
	}