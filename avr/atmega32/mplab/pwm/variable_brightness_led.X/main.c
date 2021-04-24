/* 
 * File:   main.c
 * Author: Victor Tran (github.com/victorvantran)
 *
 * Created on April 16, 2021, 2:46 PM
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


#define WGM_FAST_PWM_NI 0 
#define WGM_FAST_PWM_I 1 
#define WGM_PHASE_CORRECTED_PWM_NI 2
#define WGM_PHASE_CORRECTED_PWM_I 3

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



void init_pwm(uint8_t mode)
{
    // Rely on built-in hardware unit for wave generation instead of software
    // manual delays...
    
    // Set Direction of OC0 to output
    DDRB |= _BV(PINB3);
    
    // Set PWM Waveform mode
    if (mode == WGM_FAST_PWM_NI)
    {
        // Set fast PWM mode, non_inverted
        TCCR0 |= _BV(WGM01) | _BV(WGM00) | _BV(COM01);
    }
    else if (mode == WGM_PHASE_CORRECTED_PWM_NI)
    {
        TCCR0 |= _BV(WGM00) | _BV(COM01);
    }
    else if (mode == WGM_PHASE_CORRECTED_PWM_I)
    {
        TCCR0 |= _BV(WGM01) | _BV(WGM00) | _BV(COM01) | _BV(COM00);
    }
        else if (mode == WGM_PHASE_CORRECTED_PWM_I)
    {
        TCCR0 |= _BV(WGM00) | _BV(COM01) | _BV(COM00);
    }

    // Set clock source with pre-scaling 1 (too dangerously fast for stepper motor)
    TCCR0 |= _BV(CS00);
   
    // Set initial OCR0, which determines duty cycle
    OCR0 = 255;
}



void change_duty_cycle(void)
{
    // Change duty cycle to control brightness of LED
    uint8_t duty;
    const uint8_t delay = 2;
    
    for (duty = 0; duty < 255; duty++)
    {
        OCR0 = duty;
        _delay_ms(delay);
    }

    for (duty = 255; duty > 1; duty--)
    {
        OCR0 = duty;
        _delay_ms(delay);
    }
}



void setup(void)
{
    cli();
    
    init_UART(9600);
    init_probe();
    init_pwm(WGM_FAST_PWM_NI);
    
    sei();
}



int main(void)
{
    setup();
 
    for (;;)
    {   
        change_duty_cycle();
    }
}

