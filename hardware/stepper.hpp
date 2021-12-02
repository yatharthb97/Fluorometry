#pragma once

#include "./../pins.hpp"


//void Stepper::move(int n, unsigned int step_hperiod=5, bool disable_power=false);
//void Stepper::forward(int n, unsigned int step_hperiod=5, bool disable_power=false);
//void Stepper::reverse(int n, unsigned int step_hperiod=5, bool disable_power=false);


namespace Stepper
{
    static unsigned int total_steps = STEPPER_STEPS;

    /** @brief Initalization function for module. */
    void init()
    {
        pinMode(EN_PIN, OUTPUT);
        pinMode(DIR_PIN, OUTPUT);
        pinMode(PULSE_PIN, OUTPUT);

    }

    /** @brief Generic function that moves n-steps without setting the direction pin. */
    void move(int n, unsigned int step_hperiod=15, bool disable_power=false)
    {
        digitalWrite(EN_PIN, LOW);
        delay(2);
        
        for(unsigned int i = 0; i < n; i++)
        {
            digitalWrite(PULSE_PIN, HIGH);
            delay(step_hperiod);
            digitalWrite(PULSE_PIN, LOW);
            delay(step_hperiod);
        }

        if(disable_power)
            { digitalWrite(EN_PIN, HIGH); }
    }

    /** @brief Function to move the motor **forward** by `n` steps. */
    void forward(int n, unsigned int step_hperiod=15, bool disable_power=false)
    {

        digitalWrite(DIR_PIN, HIGH);
        delay(2);
        Stepper::move(n, step_hperiod, disable_power);
    }

    /** @brief Function to move the motor **reverse** by `n` steps. */
    void reverse(int n, unsigned int step_hperiod=15, bool disable_power=false)
    {

        digitalWrite(DIR_PIN, LOW);
        delay(2);
        Stepper::move(n, step_hperiod, disable_power);
    }
};

