/* 
 * File:   main.c
 * Author: Victor Tran (github.com/victorvantran)
 *
 * Created on April 13, 2021, 7:21 PM
 */


#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "ADC.h"

#include <stdio.h>


#define SET_BIT(p,i)  ((p) |= (_BV(i)))
#define CLR_BIT(p,i)  ((p) &= ~(_BV(i)))
#define GET_BIT(p,i)  ((p) & (_BV(i)))
#define FLIP_BIT(p,i) ((p) ^= (_BV(i)))



static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

static int uart_putchar(char c, FILE *stream) {
    if (c == '\n')
    {
        return uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
    return 0;
}

static int uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSRA, RXC);
    return UDR;
}



void init_UART(unsigned long bit_rate)
{
    // Calculate baud pre-scaler
    uint16_t baud_prescalar = (uint16_t)(((F_CPU / (bit_rate * 16UL))) - 1UL);
    
    // Set the baud rate pre-scalar. 
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


void init_debug(void)
{    
    DDRC  = 0b11111111;
    PORTC = 0b00000000;
}



void setup(void)
{
    cli();
    
    init_debug();
    init_UART(9600);
    init_ADC();
    
    sei();
}


int main(void)
{
    setup();
    for (;;)
    {   
        read_ADC(0);
        
        uint16_t do_low = adc_low;
        uint16_t do_high = adc_high;
        
        uint16_t digital_output = (do_high << 8) + do_low;
        
        printf("%d\n", digital_output);

        FLIP_BIT(PORTC, PINC0);
        _delay_ms(100);
        
    }
}



