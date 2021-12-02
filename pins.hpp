#pragma once

/** @brief This file contains pin definations. (default MCU → Arduino Mega 2560)
 * It also contains some other configuration settings. */


#define STEPPER_STEPS 200 
#define IR_SENSOR_THRESHOLD 50


//Stepper Pins
const int EN_PIN = 5;
const int DIR_PIN = 4;
const int PULSE_PIN = 3;



//IR Sensor pins
const int IR_SENSOR_READ_PIN = A1;



//Laser/LED Control Pin
const int LIGHT_CNTRL_PIN = 12;



//PMT Control Pins
const int PMT_CS_PIN = 53;
const int PMT_READ_PIN = A0;


//Dedicated DC Vibration motor control pins
/** \todo Add pins. */
const int VIB_IN_PIN = 0;
const int VIB_VCC_PIN = 0;