 /* 
 * File:   main.c
 * Author: victor
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
    
    for (;;)
    {        
        writeTransmitter("With one look!\r\n");
        writeTransmitter("To my people in the dark...\r\n");
        writeTransmitter("Still out there in the dark...\r\n");
        
        /*
        char c = getChar();
        if (c == '0')
        {
            CLR_BIT(PORTA, PINA1);
        }
        else if (c == '1')
        {
            SET_BIT(PORTA, PINA1);
        }
        */
        
        
        char* string = getString();
     
        if (strcmp(string, "on") == 0)
        {
            SET_BIT(PORTA, PINA1);
        }
        else if (strcmp(string, "off") == 0)
        {
            CLR_BIT(PORTA, PINA1);
        }
        
        free(string);
                
        FLIP_BIT(PORTA, PINA0);
        _delay_ms(1000);

    }
}



