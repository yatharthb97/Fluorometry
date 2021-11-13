#pragma once

/** @brief Module that contains code to vibrate the `sample tray`.*/
namespace Vibrator
{
	/** @brief Initalization function for module. */
	void init()
	{
		pinMode(VIB_VCC_PIN,OUTPUT);
		pinMode(VIB_IN_PIN,OUTPUT);
	}


	/** @brief Emulates vibration using stepper motors.
	 * @param time_ms Time for vibration in milliseconds. */
	void stepper_emulate(unsigned int time_ms)
	{
		const int stepper_h_period_ms = 10;
		unsigned int vib_steps = floor(float(time_ms)/float(stepper_h_period_ms * 4));

		for(unsigned int i = 0; i < vib_steps; i++)
		{
			//Oscillate 90 steps back and forth with halfperiod of 10 ms.
			Stepper::forward(90, 10);
			Stepper::reverse(90, 10);
		}
	}


	/** @brief Function that controls a dedicated DC Vibration motor.
	 * @param time_ms Time for vibration in milliseconds. */
	void dedicated_dc(unsigned int time_ms)
	{
		digitalWrite(VIB_VCC_PIN, HIGH);

		unsigned long init_time = millis();
		while((millis() - init_time) < time_ms)
		{
			digitalWrite(VIB_IN_PIN, HIGH);
			delay(2);
			digitalWrite(VIB_IN_PIN, LOW);
			delay(2);
		}

		digitalWrite(VIB_VCC_PIN, LOW);
	}
};