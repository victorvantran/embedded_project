/* 
 * File:   UART.h
 * Author: victor
 *
 * Created on April 8, 2021, 7:28 PM
 */



#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif


    
#include <avr/interrupt.h>

    
#define DEFAULT_USART_BITRATE 9600UL
#define BAUD_PRESCALAR (uint16_t)(((F_CPU / (USART_BITRATE * 16UL))) - 1)

    
static volatile unsigned char v_character = 0b00000000;


void init_UART(unsigned long USART_BITRATE)
{
    // Set the baud rate prescalar. 
    // Set the UBBRH to the higher 8-bits
    // Set the UBBRL to the lower 8-bits
    UBRRH = BAUD_PRESCALAR >> 8; 
    UBRRL = BAUD_PRESCALAR;
    
    // Enable receive and transmission
    UCSRB |= _BV(RXEN) | _BV(RXCIE) | _BV(TXEN) ;
    
    // Set URSEL (at the same time of writing) to write to the UCSRC
    // instead of the UBRRH because the registers share the same address    
    // Set character size to 8-bit by using the USCRC 
    UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
}    



ISR(USART_RXC_vect)
{
    // Receive the character by reading the UDR and put the
    // value in a non-volatile receive character variable
    char nv_r_character;
    nv_r_character = UDR;
    v_character = nv_r_character;
}



#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

