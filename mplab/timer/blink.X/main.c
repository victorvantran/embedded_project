/* 
 * File:   main.c
 * Author: Victor Tran (github.com/victorvantran)
 *
 * Created on April 14, 2021, 8:53 PM
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



void init_timer1(void)
{
    // Set the TIMSK (Timer/Counter Interrupt Mask Register):
    // Enable overflow on timer1
    TIMSK = _BV(TOIE1);

    // Seed timer1 for 1.0 second cycle on overflow
    // Formula: (clock_period * pre-scaler * ((2^#timer_bits - 1) - seed) = time_in_seconds
    TCNT1 = 34285;
    
    
    // Start timer1 with pre-scaler of 256 for long enough 1.0 second cycle
    TCCR1B = _BV(CS02);
}


ISR(TIMER1_OVF_vect)
{
    // Re-seed timer1
    TCNT1 = 34285;
    FLIP_BIT(PORTA, PINA0);
}



void init_probe(void)
{
    // Initiate output port for logic probing
    DDRA = 0xFF;
    PORTA = 0x00;
}



void setup(void)
{
    cli();
    
    init_UART(9600);
    init_timer1();
    init_probe();
    
    sei();
}



int main(void)
{
    setup();
    
    for (;;)
    {
        printf("PINA0: %i\r", GET_BIT(PORTA, PINA0));
        _delay_ms(500);
    }
}

