#pragma once

#include "pins.hpp"

namespace PMT
{
    /** @brief Reads and returns the pmt reading.
     * @param integn_time_ms Integration time over which the value is averaged. */
    int read(unsigned int integn_time_ms)
    {
        int signal, noise = 0;
        double sig_plus_noise_integrated = 0.0;
        double noise_integrated = 0.0;
        double clean_sig_integrated = 0.0;
        
        //Integration loop
        for(unsigned int i = 0; i < integn_time_ms; i++)
        {
            digitalWrite(LASER_CNTRL_PIN, HIGH);
            delayMicroseconds(500);
            
            signal = analogRead (PMT_READ_PIN);
            signal_integrated += signal;  
            
            //☼
            asm volatile ("dsb"); //Wait for GPIO to synchronise & prevent optimizations
            //☼
            
            digitalWrite(LIGHT_CNTRL_PIN, LOW);
            delayMicroseconds(500);
            
            noise = analogRead (PMT_READ_PIN);
            noise_integrated += noise;

            clean_sig_integrated +=  (signal - noise);
        }
        
        signal_integrated = signal_integrated / integn_time_ms;
        noise_integrated = noise_integrated / integn_time_ms;
        clean_sig_integrated = clean_sig_integrated / integn_time_ms;


        //TODO : Clean
        #if FLUORI_DEBUG_MESSAGES == 1
            Serial.print("signal =  ");
            Serial.print(signal_integrated);
            Serial.print("\t");
            Serial.print(noise_integrated);
            Serial.print("\t");
            Serial.println(clean_sig_integrated);
        #endif

        return(clean_sig_integrated);
    }
};