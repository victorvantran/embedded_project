/* 
 * File:   LCD_16x2.h
 * Edited: victor 
 *
 * Created on April 8, 2021, 7:28 PM
 */



#ifndef LCD_16X2_H
#define	LCD_16X2_H

#ifdef	__cplusplus
extern "C" {
#endif


#define F_CPU 8000000UL					/* Define CPU Frequency e.g. here its 8MHz */
#include <avr/io.h>						/* Include AVR std. library file */
#include <util/delay.h>					/* Include inbuilt defined Delay header file */
#include <stdlib.h>                     /* Include stdlib for itoa */


#define LCD_Data_Dir DDRB				/* Define LCD data port direction */
#define LCD_Command_Dir DDRC			/* Define LCD command port direction register */
#define LCD_Data_Port PORTB				/* Define LCD data port */
#define LCD_Command_Port PORTC			/* Define LCD command port */
#define EN PC2							/* Define Enable signal pin */
#define RW PC3							/* Define Read/Write signal pin */
#define RS PC4							/* Define Register Select (data reg./command reg.) signal pin */
#define CLEAR 0x01                      /* Define CLEAR command to clear LCD display */

void LCD_Command (char);				/* LCD command write function */
void LCD_Char (char);					/* LCD data write function */
void LCD_Init (void);					/* LCD Initialize function */
void LCD_String (char*);				/* Send string to LCD function */
void LCD_Byte(unsigned char,char*);
void LCD_String_xy (char,char,char*);	/* Send row, position and string to LCD function */



#ifdef	__cplusplus
}
#endif

#endif	/* LCD_16X2_H */

