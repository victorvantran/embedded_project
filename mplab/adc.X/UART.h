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

#define BAUD_PRESCALAR (uint16_t)(((F_CPU / (USART_BITRATE * 16UL))) - 1UL)
#define SERIAL_BUFFER_SIZE 256

    
    
static struct 
{
    uint8_t buffer[SERIAL_BUFFER_SIZE];
    uint8_t readIndex;
    uint8_t writeIndex;
} 
transmitter = {.readIndex = 0, .writeIndex = 0},
receiver = {.readIndex = 0, .writeIndex = 0};

char string_buffer[SERIAL_BUFFER_SIZE];



void init_UART(unsigned long USART_BITRATE)
{
    // Set the baud rate prescalar. 
    // Set the UBBRH to the higher 8-bits
    // Set the UBBRL to the lower 8-bits
    UBRRH = (BAUD_PRESCALAR >> 8); 
    UBRRL = BAUD_PRESCALAR;
    
    // Enable receive, transmission, and their interrupts
    UCSRB |= _BV(RXEN) | _BV(RXCIE) | _BV(TXEN) | _BV(TXCIE);
    
    // Set URSEL (at the same time of writing) to write to the UCSRC
    // instead of the UBRRH because the registers share the same address    
    // Set dataframe size to 8-bit by using the USCRC 
    UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
}    



// Receiver
char receive_char(void)
{
    // Receive a character to our circular transmitter buffer
    char character = '\0';
    
    if (receiver.readIndex != receiver.writeIndex)
    {
        character = receiver.buffer[receiver.readIndex];
        
        receiver.readIndex++;
        
        if (receiver.readIndex >= SERIAL_BUFFER_SIZE)
        {
            receiver.readIndex = 0;
        }
    }
    
    return character;
}


char* receive_string(void)
{
    // Receive a string, which is a nullchar-terminated array of char
    char character;
    for (uint16_t i = 0; i < SERIAL_BUFFER_SIZE - 1; i++)
    {
        character = receive_char();
        string_buffer[i] = character;           
        if (character == '\0') return string_buffer;
    }
    
    return string_buffer;
}


ISR(USART_RXC_vect)
{
    // If the serial received is greater than the SERIAL_BUFFER_LENGTH,
    // then we will choose the latest characters abcd...[latest characters]
    // as the buffer will wrap and the earliest characters are overwritten
    receiver.buffer[receiver.writeIndex] = UDR;
    receiver.writeIndex++;
    
    if (receiver.writeIndex >= SERIAL_BUFFER_SIZE)
    {
        receiver.writeIndex = 0;
    }
}



// Transmitter
void transmit_char(char character)
{
    // Transmit a character to our circular transmitter buffer
    transmitter.buffer[transmitter.writeIndex] = character;
    transmitter.writeIndex++;
    
    if (transmitter.writeIndex >= SERIAL_BUFFER_SIZE)
    {
        transmitter.writeIndex = 0;
    }
}


void transmit_string(char* string)
{
    // Transmit a string, which is a nullchar-terminated array of char
    for (uint16_t i = 0; i < SERIAL_BUFFER_SIZE; i++)
    {
        transmit_char(string[i]);
        if (string[i] == '\0') break;
    }
        
    if (UCSRA & (_BV(UDRE)))
    {
        UDR = 0;
    }
}



ISR(USART_TXC_vect)
{
    // If the serial transmit is greater than the SERIAL_BUFFER_LENGTH,
    // then we will choose the latest characters abcd...[latest characters]
    // as the buffer will wrap and the earliest characters are overwritten
    if (transmitter.readIndex != transmitter.writeIndex)
    {
        UDR = transmitter.buffer[transmitter.readIndex];
        transmitter.readIndex++;
        
        if (transmitter.readIndex >= SERIAL_BUFFER_SIZE)
        {
            transmitter.readIndex = 0;
        }
    }
}



#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

