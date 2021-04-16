/* 
 * File:   main.c
 * Author: Victor Tran (github.com/victorvantran)
 *
 * Created on April 14, 2021, 10:26 PM
 */


#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define SET_BIT(p,i) ((p) |= (_BV(i)))
#define CLR_BIT(p,i) ((p) &= ~(_BV(i)))
#define FLIP_BIT(p,i) ((p) ^= (_BV(i)))
#define GET_BIT(p,i) (((p) & (_BV(i))) >> i)

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
    (byte & (0x80) ? '1' : '0'), \
    (byte & (0x40) ? '1' : '0'), \
    (byte & (0x20) ? '1' : '0'), \
    (byte & (0x10) ? '1' : '0'), \
    (byte & (0x08) ? '1' : '0'), \
    (byte & (0x04) ? '1' : '0'), \
    (byte & (0x02) ? '1' : '0'), \
    (byte & (0x01) ? '1' : '0')



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



void init_icm(void)
{
    // Initiate icm flags for capturing on falling edge
    DDRD &= ~PIND6;
    PORTD |= PIND6; // pull-up
    
    TCCR1A = 0x00;
    TCCR1B = _BV(ICNC1) | _BV(CS10);
}



void print_width(void)
{
    // Have another microcontroller do fast pwm and measure the clock period (or output compare timer with OCR = n)
    uint16_t t;
    
    TIFR = _BV(ICF1);
    while (!(TIFR & (_BV(ICF1))));
    t = ICR1;
    
    TIFR = _BV(ICF1);
    while (!(TIFR & (_BV(ICF1))));
    t = ICR1 - t;
    
    printf("Width: "BYTE_TO_BINARY_PATTERN"\r", BYTE_TO_BINARY(t));
}



void setup(void)
{
    cli();
    
    init_UART(9600);
    init_probe();
    init_icm();
    
    sei();
}



int main(void)
{
    setup();
    
    for (;;)
    {
        print_width();
        _delay_ms(1000);
    }
}

