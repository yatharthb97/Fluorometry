#pragma once

#include "./../pins.hpp"
#include "stepper.hpp"

//void IRSensor::init();
//unsigned int IRSensor::read();
//bool IRSensor::is_aligned();
//void IRSensor::align(unsigned int step_halfperiod=30);

/** @brief Namespace for controlling IRSensor and related operations.
 * \dependency Macro values for Pins. */
namespace IRSensor
{
    static unsigned int Max = 0; //!< Maximum Sensor value for the current environment (set by `IRSensor::align()`)
    static unsigned int Min = 0; //!< Minimum Sensor value for the current environment (set by `IRSensor::align()`)
    
    static bool UseMaximum = false; //!< Use the Maximum as the start position (depends on the type of IR sensor).
    static unsigned int IRThreshold = IR_SENSOR_THRESHOLD;
    /** @brief Initalization function for module. */
    void init()
    {
        pinMode(IR_SENSOR_READ_PIN, INPUT);
    }

    
    /** @brief Reads sensor and returns the analog value.
     * Returns value after `NoiseCancellation`, if it is set to true. */
    unsigned int inline read()
    {
        unsigned int signal = analogRead(IR_SENSOR_READ_PIN);
        return signal;
    }

    /** @brief Returns `true` if the sensor is aligned with the start position,
     *  and vice-versa. */
    bool is_aligned()
    {
        int sensor = IRSensor::read();
        bool status =  (sensor < IRSensor::IRThreshold) * true + 
                      !(sensor < IRSensor::IRThreshold) * false;

        return IRSensor::UseMaximum * (!status) + (!IRSensor::UseMaximum) * status;
    }


    /** @brief Aligns the sensor to the START position, by exploring the rotation space. */
    void align(unsigned int step_halfperiod=30)
    {
        unsigned int steps = floor(360.0/STEPPER_STEPS);

        unsigned int sensor_val[steps] = {0};

        for(unsigned int i = 0; i < steps; i++)
        {
            sensor_val[i] = IRSensor::read();
            delay(step_halfperiod);
            Stepper::forward(1); //Forward one step
        }

        //Find Minimum & Maximum
        unsigned int max_val, min_val = sensor_val[0];
        unsigned int min_index, max_index = 0;
        for(unsigned int i = 0; i < steps; i++)
        {
            if(sensor_val[i] < min_val)
            {
                min_val = sensor_val[i];
                min_index = i;
            }

            if(sensor_val[i] > max_val)
            {
                max_val = sensor_val[i];
                max_index = i;
            }
        }

        //Set Globals
        IRSensor::Max = max_val;
        IRSensor::Min = min_val;

        //Move Motor to desired poisiton
        unsigned int align_pos = (IRSensor::UseMaximum) * max_index +
                                !(IRSensor::UseMaximum) * min_index;

        unsigned int reverse_steps = steps - align_pos;
        Stepper::reverse(reverse_steps);


        //Check alignment
        bool aligned_status = IRSensor::is_aligned();
        if(aligned_status == false)
        {
            Serial.println("# ERROR: | IRSensor > Alignment failed !");
        }

    }

};