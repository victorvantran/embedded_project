/* 
 * File:   ADC.h
 * Author: victor
 *
 * Created on April 13, 2021, 7:27 PM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <avr/interrupt.h>
#include <util/delay.h>



volatile uint8_t adc_low;
volatile uint8_t adc_high;
    
    
void init_ADC(void)
{
    // Set port direction
    DDRA  = 0b00000000;
    PORTA = 0b11111111;
    
    // Enable ADC
    ADCSRA |= _BV(ADEN);
            
    // Enable ADC_complete interrupt
    ADCSRA |= _BV(ADPS1);
    
    // Enable Auto-Trigger (default free-running)
    ADCSRA |= _BV(ADATE);
    
    // set prescalar of 64 giving 125KHz on a 8MHz oscillator
    ADCSRA |= _BV(ADIE) | _BV(ADPS2);
    
    // ADC reference voltage set to AVCC pin
    ADMUX |= _BV(REFS0);   
}



void read_ADC(char channel)
{    
    // Select channel/analog input pin
    ADMUX |= (channel & 0x0f);
    
    // Start conversion
    ADCSRA |= _BV(ADSC);
}

   

ISR(ADC_vect)
{
    // Set volatile variables to adc_low and adc_high
    adc_low = ADCL;
    adc_high = ADCH;
}



#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

