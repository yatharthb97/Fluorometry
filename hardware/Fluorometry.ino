#include "hardware/ir_sensor.hpp"
#include "tray.hpp"
#include "hardware/stepper.hpp"


unsigned int samples = 4;

void setup()
{
	Serial.begin(9600);

	Stepper::init();
	IRSensor::init();

	PMT::init();
	//PMT::useADC = true;
	PMT::set_ref_volt(500);

	IRSensor::align(); //Align to start position
}



void loop()
{
	Tray::simple_measure(samples);
}
