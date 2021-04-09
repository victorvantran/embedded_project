 /* 
 * File:   main.c
 * Author: victor
 *
 * Created on April 7, 2021, 3:21 PM
 */



#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "UART.h"
#include "LCD_16x2.h"


#define SET_BIT(p,i)  (p |= _BV(i))
#define CLR_BIT(p,i)  (p &= ~_BV(i))
#define GET_BIT(p,i)  (p & _BV(i))
#define FLIP_BIT(p,i) (p ^= _BV(i))



void init_debug(void)
{
    DDRA  = 0b11111111;
    PORTA = 0b00000000;
}


void setup(void)
{
    cli();
    
    init_debug();
    LCD_Init();
    init_UART(DEFAULT_USART_BITRATE);
    
    sei();
}


int main(void)
{
    setup(); 
    
    char lcd_row[16];
    char nv_dataframe;
    
    for (;;)
    {
        // Save the volatile variable to a non-volatile variable
        nv_dataframe = v_dataframe;
        
        // Send the character to the transmit buffer (parallel in)
        UDR = nv_dataframe;
        // No need need to wait for TXC because RXC complete on other end implies TXC
        
        // Display character to LCD
        LCD_Command (0x01);    
        LCD_Byte(nv_dataframe, lcd_row);
        
        // Toggle bit for logic probing
        FLIP_BIT(PORTA, PINA1);
        _delay_ms(1000);

    }
}



