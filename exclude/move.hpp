#pragma once
#include "pins.hpp"
#include "tray.hpp"

/** @brief Highest level of interface for stage motion. */



namespace Move
{
	typedef unsigned int stage_pos_t;
	using stage_pos_t = Move::stage_pos_t;

	
	stage_pos_t static pos = 0; //!< Current position of the motion stage.
	stage_pos_t static pos_y = 0; //!< Current position of the motion stage.
	

	stage_pos_t static last_pos = 0; //!< Last position of the motion stage.
	stage_pos_t static last_pos_Y = 0; //!< Last position of the motion stage.
	

	stage_pos_t static stage_len_x = 0; //!< Total length of the transalation/rotational stage. 
	stage_pos_t static stage_len_y = 0; //!< Total length of the transalation/rotational stage. 


	/** @brief Function to move object to new stage position. */
	void to(stage_pos_t new_pos)
	{
		if(new_pos != Move::pos)
		{
			digitalWrite(LED_POWER_PIN, LOW);

			stage_pos_t forward_steps = Move::stage_len_x - (Move::pos - new_pos);

			Motor::forward(forward_steps);
			Move::last_pos = pos;
			Move::pos = new_pos;

			digitalWrite(LED_POWER_PIN, HIGH);
		}
	}

	/*
			----------|
					  |
		    |---------|
			|
			|---------|
					  |
			|---------|


	*/


	void to_xy(stage_pos_t new_pos_x, stage_pos_t new_pos_y)
	{
		if(new_pos_x != Move::pos && new_pos_x != Move::pos_y)
		{
			digitalWrite(LED_POWER_PIN, LOW);

			int steps_x =  (new_pos_x - Move::pos);
			int steps_y =  (new_pos_y - Move::pos_y);


			Motor::x_move(steps_x);
			Motor::y_move(steps_y);

			
			//Update State of Move Module			
			Move::last_pos = Move::pos;
			Move::pos = new_pos_x;

			Move::last_pos_y = Move::pos_y;
			Move::pos_y = new_pos_y;

			digitalWrite(LED_POWER_PIN, HIGH);
		}
	}
};


class DCMotor
{	
	DCMotor()
	{

	}

	
	/** @brief Moves the motor after resolving the direction of the motor. */
	move()
	{}

	/** @brief Moves the motor to the designated "foraward" direction. */
	forward()
	{}

	/** @brief Moves the motor to the designated "reverse" direction. */
	reverse()
	{}



	/**
	 * @brief Set range of motion for motor, after which 
	 * the motor discards calls for any further movement. */
	void set_range()
	{}


	void oscillate(unsigned int tofro_counts, unsigned int period_ms)
	{}

	/** @brief Returns the ratio of linear movement in mm per rotation. */
	void linear_motion_ratio()


	/** @brief The gear ratio determines the step sizing and the realtion 
	 * between the actual range of motion and the shaft's range of motion. */
	void set_gear_ratio(unsigned int motor_gear, unsigned int drive_gear)
	{

	}

public:

	/** @brief Generic code that executes motion for both directions. */
	drive()

};