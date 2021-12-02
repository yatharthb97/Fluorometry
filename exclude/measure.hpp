#pragma once

extern bool measure_flag;

namespace Measure
{
	
	const unsigned int avgtime_ms = 4000;


	//-> EXPERIMENTAL
	void enable()
	{
		noInterrupts();
		measure_flag = true;
		interrupts();
	}
	void disable()
	{
		noInterrupts();
		measure_flag = false;
		interrupts();
	}
	//-> EXPERIMENTAL



	void start()
	{
		Move::align();
		
		for(unsigned int i = 0; < i < Tray::CAPACITY; i++)
		{
			if(Tray::has_sample)
			{
				Tray::value[i] = PMT::read(Measure::avgtime_ms);
				Move::to(Tray::stage_pos[i]);
				Serial.print(Tray::value[i]);
				Serial.print('\t');
			}

			Serial.print('\n');
			delay(50);

		}
		
	}

	void end()
	{
		Measure::send_data();
	}

};