#pragma once

#include "./../pins.hpp"

#include <MCP48xx.h>
#include <ADS1X15.h>

namespace PMT
{
    static ADS1115 ADC(0x4A); //!< Specialized ADC for reading PMT signals (Connected to SDA pin)
    static MCP4822 MCP(PMT_CS_PIN); //!< DAC used to set the PMT reference voltage
    static bool useADC = false; //!< Flag that determines whether to use the specialized ADC(16bit) to read PMT signals.
    static unsigned int max_ref_volt = 850; //!< Highest Reference Voltage allowed.

    /** @brief Initalize the basic PMT operations. */
    void init()
    {
        pinMode(PMT_READ_PIN, INPUT);

        PMT::MCP.init();
        PMT::MCP.setGainA(MCP4822:: High);
        PMT::MCP.setGainB(MCP4822:: Low);
        PMT::MCP.turnOnChannelA();          //Turn ON channel A
        PMT::MCP.setVoltageA(000);          //500= 0.505 volt controll voltage
        PMT::MCP.updateDAC();               //Update DAC

    }

    /** @brief Initalized the 16-bit ADC for reading the PMT signals. */
    void init_ADC()
    {
        PMT::ADC.begin();
        PMT::ADC.setGain(0);      // 6.144 volt
        PMT::ADC.setDataRate(7);  // fast
        PMT::ADC.setMode(0);      // continuous mode
        PMT::ADC.readADC(0);      // first read to trigger
    }

    /** @breif Set the reference voltage for PMT to the value `volt`.
     * \attention If the value passed is greater than the `PMT::max_ref_volt`, then an error 
     * is generated and the call is ignored.*/
    void set_ref_volt(unsigned int volt)
    {
        if(volt > PMT::max_ref_volt)
        {
            Serial.println("# ERROR : | PMT > Safety mechanism prevented setting of a high reference voltage.")
        }

        else
        {
            PMT::MCP.setVoltageA(volt); //500= 0.505 volt controll voltage
            PMT::MCP.updateDAC(); //updating DAC
            Serial.print("# | PMT > Reference Voltage = ");
            Serial.println(volt);
        }
    }


    /** @brief Reads and returns the pmt reading.
     * @param integn_time_ms Integration time over which the value is averaged. */
    int read(unsigned int integn_time_ms)
    {
        uint16_t signal, noise = 0;                      // Instantaneous
        double sig_plus_noise_integrated = 0.0;     // Integrated values ↓
        double noise_integrated = 0.0;
        double clean_sig_integrated = 0.0;
        
        //Integration loop
        for(unsigned int i = 0; i < integn_time_ms; i++)
        {
            digitalWrite(LIGHT_CNTRL_PIN, HIGH);
            delayMicroseconds(500);
            
            if (PMT::useADC)
                { signal = PMT::ADC.getValue(); }
            else
                { signal = analogRead(PMT_READ_PIN); }
            
            sig_plus_noise_integrated += signal;  
            
            //☼
            asm volatile ("dsb"); //Wait for GPIO to synchronise & prevent optimizations
            //☼
            
            digitalWrite(LIGHT_CNTRL_PIN, LOW);
            delayMicroseconds(500);
            
            if (PMT::useADC)
                { noise = PMT::ADC.getValue(); }
            else
                { noise = analogRead(PMT_READ_PIN); }
            noise_integrated += noise;

            clean_sig_integrated +=  (signal - noise);
        }
        
        sig_plus_noise_integrated = sig_plus_noise_integrated / double(integn_time_ms);
        noise_integrated = noise_integrated / double(integn_time_ms);
        clean_sig_integrated = clean_sig_integrated / double(integn_time_ms);


        //TODO : Clean
        //Serial.print("signal =  ");
        //Serial.print(sig_plus_noise_integrated);
        //Serial.print("\t");
        //Serial.print(noise_integrated);
        //Serial.print("\t");
        //Serial.println(clean_sig_integrated);
       

        return(clean_sig_integrated);
    }
};
