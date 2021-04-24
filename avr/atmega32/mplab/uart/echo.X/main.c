 /* 
 * File:   main.c
 * Author: Victor Tran (github.com/victorvantran)
 *
 * Created on April 7, 2021, 3:21 PM
 */



#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "UART.h"


#define SET_BIT(p,i)  ((p) |= (_BV(i)))
#define CLR_BIT(p,i)  ((p) &= ~(_BV(i)))
#define GET_BIT(p,i)  ((p) & (_BV(i)))
#define FLIP_BIT(p,i) ((p) ^= (_BV(i)))



void init_debug(void)
{
    DDRA  = 0b11111111;
    PORTA = 0b00000000;
}


void setup(void)
{
    cli();
    
    init_debug();
    init_UART(DEFAULT_USART_BITRATE);
    
    sei();
}


int main(void)
{
    setup(); 
    
    char* echo = string_buffer;
  
    for (;;)
    {         
        receive_string();
     
        if (echo[0] != '\0')
        {
            transmit_string(echo);
        }
      
        FLIP_BIT(PORTA, PINA0);
        _delay_ms(100);
    }
}



