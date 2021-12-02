#pragma once

class Flouorometer
{
public:

	Flouorometer()
	{
		void virtual sense() = 0;
		void virtual move() = 0;
		void virtual measure() = 0;

	}
};