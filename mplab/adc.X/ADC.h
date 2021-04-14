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



void init_ADC(void)
{
    // Set port direction
    DDRA  = 0b00000000;
    PORTA = 0b11111111;
    
    // Enable ADC with a prescalar of 64 giving 125KHz.
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1);
    
    // ADC reference voltage set to AVCC pin
    ADMUX |= _BV(REFS0);   
}


int read_ADC(char channel)
{
    int a_in, a_in_low;
    
    ADMUX |= (channel & 0x0f);
    
    ADCSRA |= (1 << ADSC);
    
    while ((ADCSRA & (_BV(ADIF))) == 0);
    _delay_us(10);
    
    a_in_low = (int)ADCL;
    a_in = ((int)ADCH << 8) + a_in_low;
    return a_in;
}
   

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

