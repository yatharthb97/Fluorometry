//#pragma once

#include "./hardware/stepper.hpp"
#include "./hardware/pmt.hpp"



/** @brief Responsible for selection of sample trays and book-keeping. */
namespace Tray
{
	using stage_pos_t = unsigned int;


	static constexpr const unsigned int CAPACITY = 4;

	static bool has_sample[CAPACITY] = {false};
	static stage_pos_t stage_pos[CAPACITY] = {0};
	static double reading[CAPACITY] = {0.0};
	static unsigned int size = 0;
	


	/** @brief Clears all the trays. */
	void clear()
	{
		for(unsigned int i = 0 ; i < Tray::CAPACITY; i++)
		{
			Tray::has_sample[i] = false;
			Tray::size = 0;
		}
	}

	/** @brief Functions to select a particular tray. */
	void select(unsigned int idx) 
	{ 
		Tray::size++; 
		Tray::has_sample[idx] = true;  
	}

	/** @brief Discrete call functions.*/
	void select1() { Tray::select(0); }
	void select2() { Tray::select(1); }
	void select3() { Tray::select(2); }
	void select4() { Tray::select(3); }
	void select5() { Tray::select(4); }
	void select6() { Tray::select(5); }
	void select7() { Tray::select(6); }
	void select8() { Tray::select(7); }


	/** @brief Calculates the stage positons based on the last updated value of `Move::StageLength`. */
	void calc_positions()
	{
		unsigned int incr = floor(Stepper::total_steps / Tray::CAPACITY);
		
		Tray::stage_pos[0] = 0; //Sample at start (alignment position) is defined as Sample 1.
		
		for(unsigned int i = 1; i < Tray::CAPACITY; i++)
		{	
			Tray::stage_pos[i] = incr * i;
		}
	}

	/** @brief Measures the `samples` number of trays that are even and equally spaced.
	 * \warning Truncates the value `samples` to the nearest (floored) even value. */
	void simple_measure(unsigned int samples)
	{
		samples = (samples/2)*2; //Truncation to even number
		unsigned int inc = Stepper::total_steps / samples;

		//Assume that the tray is aligned.
		Serial.print(PMT::read(4000));
		Serial.print('\t');
		
		for(unsigned int i = 1; i < samples-1; i++)
		{
			Stepper::forward(inc);
			Serial.print(PMT::read(4000));
			Serial.print('\t');
		}

		Stepper::forward(inc);
		Serial.print(PMT::read(4000));
		Serial.print('\n');
	}
};



