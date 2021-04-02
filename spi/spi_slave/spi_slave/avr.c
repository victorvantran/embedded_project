/*
 * avr.c
 *
 * Created: 4/2/2021 12:34:01 PM
 * Author : Victor
 */ 

#include "avr.h"



void avr_wait(unsigned short msec)
{
	// Set prescalar of the clock to 64
	TCCR0 = (1 << CS01) | (1 << CS00); // 3
	while (msec--) {
		// Seed the timer/counter register
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		// Set the timer/counter interrupt flag
		SET_BIT(TIFR, TOV0);
		// Wait for the timer/counter interrupt flag to toggle low
		while (!GET_BIT(TIFR, TOV0));
	}
	// Set prescalar to no prescale
	TCCR0 = 0;
}
