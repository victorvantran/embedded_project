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
#include <string.h>
    
#define DEFAULT_USART_BITRATE 9600UL
#define BAUD_PRESCALAR (uint16_t)(((F_CPU / (USART_BITRATE * 16UL))) - 1UL)
#define SERIAL_BUFFER_SIZE 256
    

    
static struct 
{
    uint8_t buffer[SERIAL_BUFFER_SIZE];
    uint8_t readIndex;
    uint8_t writeIndex;
} 
transmitter = {.buffer = {0}, .readIndex = 0, .writeIndex = 0},
receiver = {.buffer = {0}, .readIndex = 0, .writeIndex = 0};



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


// Receive
char getChar(void)
{
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



char* getString(void)
{
    char character;
    char* string = calloc(SERIAL_BUFFER_SIZE, sizeof(char));
    for (uint16_t i = 0; i < SERIAL_BUFFER_SIZE - 1; i++)
    {
        character = getChar();
        string[i] = character;
        if (character == '\0') return string; // "string^@" (yes + Ctrl + @) to send null char
    }
    
    string[SERIAL_BUFFER_SIZE - 1] = '\0';
 
    return string;
}



ISR(USART_RXC_vect)
{
    receiver.buffer[receiver.writeIndex] = UDR;
    receiver.writeIndex++;
    
    if (receiver.writeIndex >= SERIAL_BUFFER_SIZE)
    {
        receiver.writeIndex = 0;
    }
}



// Transmit
void appendTransmitter(char character)
{
    transmitter.buffer[transmitter.writeIndex] = character;
    transmitter.writeIndex++;
    
    if (transmitter.writeIndex >= SERIAL_BUFFER_SIZE)
    {
        transmitter.writeIndex = 0;
    }
}


void writeTransmitter(char* string)
{
    for (uint16_t i = 0; i < strlen(string); i++)
    {
        appendTransmitter(string[i]);
    }
    
    appendTransmitter(0);
    
    if (UCSRA & (_BV(UDRE)))
    {
        UDR = 0;
    }
}



ISR(USART_TXC_vect)
{
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

