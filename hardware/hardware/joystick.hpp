#pragma once

/** @brief Class that interfaces a 3 pin joystick. Reads the x and y position and a click switch. */
class JoyStick
{
public:

	int map_x = 0; //!< Mapped position of the joystick along x-axis
	int map_y = 0; //!< Mapped position of the joystick along y-axis


	unsigned int xPIN = 999; //!< Pin that reads the x-axis
	unsigned int yPIN = 999; //!< Pin that reads the y-axis
	unsigned int swPIN = 999; //!< Pin that reads the switch state

	volatile bool switch_state = false;			//!< Current switch state
	void(*switch_executor)() = JoyStick::blank;	//!< Function that is executed when switch is pressed.

	/** @brief Constructor that initalizes the pins of the joysticks. */
	JoyStick(unsigned int xPIN, unsigned int yPIN, unsigned int swPIN): xPIN(xPIN), yPIN(yPIN), swPIN(swPIN)
	{
		pinMode(xPIN, INPUT);
		pinMode(yPIN, INPUT);
		pinMode(swPIN, INPUT_PULLUP);
	}

	/** @brief Sets the joystick button to an interrupt.
	 * \param The optional function, if passed, is executed with every click. */
 	void set_sw_trigger(void(*switch_executor)() = JoyStick::blank)
 	{
		this->switch_executor = switch_executor;
		attachInterrupt(digitalPinToInterrupt(swPIN), toggle_switch, RISING);
	}

  	/** @brief Update the joystick states - x & y positions and switch states. */
	void read()
	{
		x_pos = analogRead(xPIN);
		y_pos = analogRead(yPIN);
		switch_state = digitalRead(swPIN);
		map_x = map(x_pos, 0, 1023, -512, 512);
		map_y = map(y_pos, 0, 1023, -512, 512);
	}


	/** @brief Returns the position of the joystick as an array of size two.  */
	int* pos()
	{
		static pos[2] = {map_x, map_y};
		return pos;
	}


	/** @brief Blank function that does nothing.*/
	static void blank() { return; }

private:

	/** @brief Toggle switch state and execute `switch_executor`.*/
	void toggle_switch()
	{
		switch_state = !switch_state;
		switch_executor();
	}

	unsigned int x_pos = 0; //!< Potentiometer position along x-axis
	unsigned int y_pos = 0; //!< Potentiometer position along y-axis

}