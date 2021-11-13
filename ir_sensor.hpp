#pragma once

#include "pins.hpp"
#include "stepper.hpp"

/** @brief Namespace for controlling IRSensor and related operations.
 * \dependency Macro values for Pins. */

namespace IRSensor
{
    static unsigned int Max = 0; //!< Maximum Sensor value for the current environment (set by `IRSensor::align()`)
    static unsigned int Min = 0; //!< Minimum Sensor value for the current environment (set by `IRSensor::align()`)
    
    static bool UseMaximum = false; //!< Use the Maximum as the start position (depends on the type of IR sensor).
    
    //static bool NoiseCancellation = false; //!< Enables noise cancellation.

    /** @brief Initalization function for module. */
    void init()
    {
        pinMode(IR_SENSOR_READ_PIN, OUTPUT);
    }

    
    /** @brief Reads sensor and returns the analog value.
     * Returns value after `NoiseCancellation`, if it is set to true. */
    unsigned int inline read()
    {
        unsigned int signal = analogRead(IR_SENSOR_READ_PIN);

        /*if(IRSensor::NoiseCancellation) //HW-201 has a combined VCC pin (emitter cannot be seperately turned off).
        {
            digitalWrite(IR_EMITTER_POWER_PIN, LOW);
            delay(5); // Turn off emitter

            unsigned int noise = analogRead(IR_SENSOR_READ_PIN);
            
            delay(5); // Turn on emitter
            digitalWrite(IR_EMITTER_POWER, HIGH);
            
            signal -= noise;
        }*/

        return signal;
    } 

    /** @brief Returns `true` if the sensor is aligned with the start position,
     *  and vice-versa. */
    bool is_aligned()
    {
        int sensor = IRSensor::read(IR_SENSOR_READ_PIN);
        bool status =  (sensor < IR_SENSOR_THRESHOLD) * true + 
                      !(sensor < IR_SENSOR_THRESHOLD) * false;

        return IRSensor::UseMaximum * (!status) + (!IRSensor::UseMaximum) * status;
    }


    /** @brief Aligns the sensor to the START position, by exploring the rotation space. */
    void align()
    {
        unsigned int steps = floor(360.0/STEPPER_STEPS);

        unsigned int sensor_val[steps] = {0};

        for(unsigned int i = 0; i < steps; i++)
        {
            sensor_val[i] = IRSensor::read(IR_SENSOR_READ_PIN);
            delay(30);
            Stepper::forward(1); //Forward one step
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


        //Check alignment
        bool aligned_status = IRSensor::is_aligned();
        if(aligned_status == false)
        {
            Error::assert("IRSensor : Alignment failed !");
        }

    }
};