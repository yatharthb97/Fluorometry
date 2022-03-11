#pragma once
#include "pins.hpp"

namespace DCMotor
{
	/** @brief States of the DCMotor {forward, reverse}. */
	enum motor_states : uint8_t
	{
		fwd = 0,
		rev = 1
	}


	uint8_t power = 255; //!< Power given to the motor.
	unsigned int pins[2] = {FWD_PIN, REV_PIN}; //!< List of pins {forward, reverse}.
	int dir = 0;  //!< Direction of motion -> {0 -> forward, 1 -> reverse}.


	/** @brief Sets the pinMode for the `FWD_PIN` and `REV_PIN`.*/
	void init()
	{
		pinMode(FWD_PIN, OUTPUT);
		pinMode(REV_PIN, OUTPUT);
	}


	/** @brief Sets the direction of the DCMotor. */
	void set_dir(DCMotor::motor_states state)
	{
		DCMotor::dir = state;
	}


	/** @brief Gives the active pin for that particular direction.
	 * */
	void headingPIN()
	{
		return pins[dir];
	}


	/** @brief Gives the pin that is not in the active direction. */
	void lowPIN()
	{
		return pin[!dir];
	}


	/** @brief Start the motor with the given power (optional). 
	 * It operates in the pre-set direction. */
	void start(uint8_t power=255)
	{
		analogWrite(DCMotor::headingPIN(), power);
		analogWrite(DCMotor::lowPIN(), 0);
	}

	/** @brief Stop the DCMotor. */
	void stop()
	{
		analogWrite(DCMotor::headingPIN(), 0);
	}



};