#pragma once



/** @brief Control layer object for LimitSwitches. The limit switch toggles 
 * an internal state boolean and can also change an external state boolean.*/
class LimitSwitch
{
private:
	volatile bool extern_placehold = false; //!< Indicates if the limit was released after being asserted
	unsigned int time_assert_ms = 0; 		//!< Store of the last trigger time for debouncing.
	bool assert_limit_on = HIGH; 			//!< Assert limit on either {HIGH or LOW}

	/** @brief Interrupt handler for */
	void isr_changedetected()
	{ 
		if(millis() - time_assert_ms < debounce_time_ms)
		{
			limit_asserted = (digitalRead(pin) == assert_limit_on);
			if(!limit_asserted)
			{
				limit_asserted = true;
				//extern_flag = volatile(true);
				time_assert_ms = millis();
			}

			else
			{
				limit_asserted = false;
				//extern_flag = volatile(false);
			}	
		}
	}

public:
	volatile bool* extern_flag = &extern_placehold; //!< External flag that is toggled with limit switch
	unsigned int debounce_time_ms = 100;
	unsigned int pin = 999;
  volatile bool limit_asserted = false;   //!< Indicates that limit was asserted
	/** @brief Constructor that takes the pin number of the limit switch 
	 * and attaches an interrupt on tha pin to look for limit trigger. 
	 * \param asser_limit_on can be either `HIGH` or `LOW`. */
	LimitSwitch(const unsigned int limit_pin, unsigned int assert_limit_on_): pin(limit_pin), assert_limit_on(assert_limit_on_)
	{
		//static_assert(assert_limit_on_ == LOW || assert_limit_on_ == HIGH, "`assert_limit_on_` can either be `HIGH` or `LOW`.");
		pinMode(pin, INPUT);
	}
  void on()
  {
    attachInterrupt(digitalPinToInterrupt(pin), isr_changedetected, CHANGE);
  }

	void set_limit_flag(volatile bool* flag)
	{
		extern_flag = flag;
	}

};
