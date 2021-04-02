/*
 * spi_master.c
 *
 * Created: 4/2/2021 12:19:23 PM
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


void initSPIMaster(void)
{
	// Before transferring data, make sure the master and slave have the proper settings
	
	// Set MOSI, SCK, and SS as output lines
	DDR_SPI = (1<<SCK) | (0<<MISO) | (1<<MOSI) | (1<<SS);
	// Enable master SPI, Master, at SCK rate of fck/16 on the SPI Control Register (SPCR)
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
	// Set SS to high (high indicates deactivated SPI)
	SET_BIT(PORT_SPI, SS);
}


void transmitSPIMaster(char data)
{
	// Set SS to low (enable communication)
	CLR_BIT(PORT_SPI, SS);
	// Set/Write to SP Data Register (SPDR)
	SPDR = data;
	testBlink();
	// Wait for transmission to complete (as indicated by the SP Transmission Interrupt Flag)
	while (!(SPSR & (1<<SPIF)));
	// Set SS to high (disable communication)
	SET_BIT(PORT_SPI, SS);
}



int main(void)
{
	initSPIMaster();
	while (1)
	{
		transmitSPIMaster(0b10011101);
	}
}