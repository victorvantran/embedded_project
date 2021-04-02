/*
 * spi_slave.c
 *
 * Created: 4/2/2021 12:52:16 PM
 * Author : Victor
 */ 



#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "avr.h"

// Define helper macros
#define SET_BIT(p,i) ((p) |=  (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) &   (1 << (i)))

// Define the Alternate Functions of PortB (for SPI)
#define SCK PB7
#define MISO PB6
#define MOSI PB5
#define SS PB4

#define DDR_SPI DDRB
#define PORT_SPI PORTB



void testBlink(void)
{
	// Wiggle LEDs
	const int lightPin = 7;
	CLR_BIT(PORTD, lightPin);
	avr_wait(500);
	SET_BIT(PORTD, lightPin);
	avr_wait(500);
}


void initSPISlave(void)
{
	// Before transferring data, make sure the master and slave have the proper settings
	
	// Set MOSI, SCK, and SS as input lines. Set MISO as output line
	DDR_SPI = (0<<SCK) | (1<<MISO) | (0<<MOSI) | (0<<SS);
	// Enable master SPI (default slave) May also explicitly set CPOL and CPHA[!]
	SPCR = (1<<SPE) | (1<<SPR0);
}


char recieveSPISlave(void)
{
	// Wait for reception to complete (as indicated by the SP Transmission Interrupt Flag)
	while (!(SPSR & (1<<SPIF)));
	// Return the data register
	return SPDR;
}



int main(void)
{
	
	initSPISlave();
	while (1)
	{
		char data = recieveSPISlave();
		
		const int lightPin = 7;
		if (data == 0b10011101)
		{
			SET_BIT(PORTD, lightPin);
		}
		else
		{
			CLR_BIT(PORTD, lightPin);
		}
	}
}
