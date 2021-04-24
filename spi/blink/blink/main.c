/*
 * blink.c
 *
 * Created: 4/9/2021 10:35:04 PM
 * Author : Victor
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>


#define SET_BIT(p,i) ((p) |= _BV(i))
#define CLR_BIT(p,i) ((p) &= ~_BV(i))


int main(void)
{
    /* Replace with your application code */
	DDRA = 0b11111111;
	
    while (1) 
    {
		SET_BIT(PORTA, PINA0);
		_delay_ms(500);
		CLR_BIT(PORTA, PINA0);
		_delay_ms(500);
    }
}

