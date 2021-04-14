/* 
 * File:   main.c
 * Author: victor
 *
 * Created on April 13, 2021, 7:21 PM
 */


#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "UART.h"
#include "ADC.h"


#define SET_BIT(p,i)  ((p) |= (_BV(i)))
#define CLR_BIT(p,i)  ((p) &= ~(_BV(i)))
#define GET_BIT(p,i)  ((p) & (_BV(i)))
#define FLIP_BIT(p,i) ((p) ^= (_BV(i)))



void init_debug(void)
{    
    DDRC  = 0b11111111;
    PORTC = 0b00000000;
}


void setup(void)
{
    cli();
    
    init_debug();
    init_UART(DEFAULT_USART_BITRATE);
    init_ADC();
    
    sei();
}


int main(void)
{
    setup();
    
    char voltage_reading[16];
    
    for (;;)
    {   
        read_ADC(0);
        
        uint16_t do_low = adc_low;
        uint16_t do_high = adc_high;
        
        uint16_t digital_output = (do_high << 8) + do_low;
        
        itoa(digital_output, voltage_reading, 10);        
        transmit_string(voltage_reading);     
        transmit_string("\n\r\0");
        
        FLIP_BIT(PORTC, PINC0);
        _delay_ms(100);
    }
}



