#pragma once

#include "pins.hpp"

/** @brief Namespace for controlling IRSensor and related operations.
 * \dependency Macro values for Pins. */

namespace IRSensor
{
    static unsigned int Max = 0; //!< Maximum Sensor value for the current environment (set by `IRSensor::align()`)
    static unsigned int Min = 0; //!< Minimum Sensor value for the current environment (set by `IRSensor::align()`)
    static bool UseMaximum = false; //!< Use the Maximum as the start position (depends on the type of IR sensor).
    static bool NoiseCancellation = false; //!< Enables noise cancellation.

    /** @brief Reads sensor and returns the analog value.
     * Returns value after `NoiseCancellation`, if it is set to true. */
    unsigned int inline read()
    {
        unsigned int signal = analogRead(IR_SENSOR_READ_PIN);

        if(IRSensor::NoiseCancellation)
        {
            digitalWrite(IR_EMITTER_POWER, LOW);
            delay(5); // Turn off emitter

            unsigned int noise = analogRead(IR_SENSOR_READ_PIN);
            
            delay(5); // Turn on emitter
            digitalWrite(IR_EMITTER_POWER, HIGH);
            
            signal -= noise;
        }

        return signal;
    } 

    /** @brief Returns `true` if the sensor is aligned with the start position,
     *  and vice-versa. */
    bool is_aligned()
    {
        int sensor = IRSensor::read(IR_SENSOR_READ_PIN);
        return (sensor < IR_SENSOR_LOWER_THRESHOLD) * true + 
              !(sensor < IR_SENSOR_LOWER_THRESHOLD) * false;
    }


    /** @brief Aligns the sensor to the START position, by exploring the rotation space. */
    void align()
    {
        double step_angle = STEPPER_RES_DEGREE;
        int steps = floor(360.0/step_angle);

        unsigned int sensor_value[steps] = {1024};

        for(unsigned int i = 0; i < steps; i++)
        {
            sensor_val[i] = IRSensor::read(IR_SENSOR_READ_PIN);
            delay(30);
            Stepper::fforward(1); //Forward one step
        }

        //Find Minimum & Maximum
        unsigned int max_val, min_val = sensor_val[0];
        unsigned int max_index, max_index = 0;
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

        bool aligned_status = IRSensor::is_aligned();


        if(aligned_status == false)
        {
            Error::assert("IRSensor : Alignment failed !");
        }

    }
};