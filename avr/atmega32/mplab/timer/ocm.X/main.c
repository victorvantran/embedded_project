/* 
 * File:   main.c
 * Author: Victor Tran (github.com/victorvantran)
 *
 * Created on April 15, 2021, 10:03 PM
 */


#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SET_BIT(p,i) ((p) |= (_BV(i)))
#define CLR_BIT(p,i) ((p) &= ~(_BV(i)))
#define FLIP_BIT(p,i) ((p) ^= (_BV(i)))
#define GET_BIT(p,i) (((p) & (_BV(i))) >> i)




static int uart_putchar(char c, FILE *stream) 
{
    if (c == '\n')
    {
        return uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
    return 0;
}


static int uart_getchar(FILE *stream) 
{
    loop_until_bit_is_set(UCSRA, RXC);
    return UDR;
}


static FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);


void init_UART(unsigned long bit_rate)
{
    // Calculate baud pre-scaler
    uint16_t baud_prescalar = (uint16_t)(((F_CPU / (bit_rate * 16UL))) - 1UL);
    
    // Set the baud rate pre-scaler. 
    // Set the UBBRH to the higher 8-bits
    // Set the UBBRL to the lower 8-bits
    UBRRH = (baud_prescalar >> 8); 
    UBRRL = baud_prescalar;
    
    // Enable receive, transmission
    UCSRB |= _BV(RXEN) | _BV(TXEN);
    
    // Set URSEL (at the same time of writing) to write to the UCSRC
    // instead of the UBRRH because the registers share the same address    
    // Set data-frame size to 8-bit
    UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
    
    // Set stream
    stdin = stdout = &uart_io;
}



void init_probe(void)
{
    // Initiate output port for logic probing
    DDRA = 0xFF;
    PORTA = 0x00;
}



void init_ocm_normal(void)
{
    // Normal mode: the timer resets on overflow
    // Set PINB3/OC0 to output
    DDRB |= _BV(PINB3);
    
    // Set output compare register
    OCR0 = 128;
    
    // Set Compare Output Mode to toggle on compare match,
    // and Start the timer with no pre-scaler.
    TCCR0 |= _BV(COM00) | _BV(CS00);
}


void init_ocm_ctc(void)
{
    // Clear timer on compare mode: the timer resets on Output Compare Match
    
    // Set PINB3/OC0 to output
    DDRB |= _BV(PINB3);
    
    // Set output compare register
    OCR0 = 200;
    
    // Set Timer mode to Clear Timer on Compare Match
    TCCR0 |= _BV(WGM01);
    
    // Set Compare Output Mode to toggle on compare match,
    // and Start the timer with pre-scalar 1024.
    TCCR0 |= _BV(COM00) | _BV(CS02) | _BV(CS00);
}


void setup(void)
{
    cli();
    
    init_UART(9600);
    init_probe();
    init_ocm_ctc();
    
    sei();
}



int main(void)
{
    setup();

    for (;;)
    {   
        _delay_ms(100);
    }
}

