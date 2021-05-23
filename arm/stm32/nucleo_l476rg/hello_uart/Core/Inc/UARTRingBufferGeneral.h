/*
 * UARTRingBufferGeneral.h
 *
 *  Created on: May 22, 2021
 *      Author: victor
 */

#ifndef INC_UARTRINGBUFFERGENERAL_H_
#define INC_UARTRINGBUFFERGENERAL_H_


/* APPLICATION PROGRAMMER */
#include "stm32l4xx_hal.h"

#define UART_BUFFER_SIZE 128UL

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

#define uart1 &huart1
#define uart2 &huart2


/* IMPLEMENTATION */
typedef struct
{
	unsigned char puBuffer[UART_BUFFER_SIZE];
#if UART_BUFFER_SIZE <= 256UL
		volatile uint8_t uHeadIndex;
		volatile uint8_t uTailIndex;
#elif UART_BUFFER_SIZE <= 65536UL
		volatile uint16_t uHeadIndex;
		volatile uint16_t uTailIndex;
#else
		volatile uint32_t uHeadIndex;
		volatile uint32_t uTailIndex;
#endif
} RingBufferHandle_t;


typedef struct
{
	UART_HandleTypeDef *pxUARTHandle;
	RingBufferHandle_t xRXRingBuffer;
	RingBufferHandle_t xTXRingBuffer;
} UARTRingBufferHandle_t;


/* Initialize ring buffer */
void vInitUARTRingBuffers(void);

void vInitUARTRingBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer, UART_HandleTypeDef *uartHandle);


/* */
void vPutCharRXBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer, unsigned char c);

/* */
void vPutCharTXBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer, unsigned char c);


/* Reads the unsigned char data from the rxRingBuffer and increment that tailIndex of said buffer
 * head == tail implies there is no unprocessed/new data in the rxRingBuffer yet.
 * Thus, the rxRingBuffer does not read any data into the passed character in this implementation.
 * Returns 0 for failure read and returns 0 for successful read.
 * */
uint8_t xReadUART(UARTRingBufferHandle_t *pxUARTRingBuffer, unsigned char *c);

/* Writes the unsigned char data into the txRingBuffer and increment that tailIndex of said buffer
 * head == tail implies unprocessed/new data filled the entire txRingBuffer.
 * Thus, the txRingBuffer refuses to overwrite/put data in this implementation.
 * Returns 0 for failure write and returns 0 for successful write.
 * */
uint8_t xWriteUART(UARTRingBufferHandle_t *pxUARTRingBuffer, unsigned char c);


/* Write string to txRingBuffer */
uint8_t xWriteStringUART(UARTRingBufferHandle_t *pxUARTRingBuffer, const unsigned char *s);


/* Gets the number of readable/processable/new characters currently in the rxRingBuffer */
uint32_t uGetNumReadableCharRXBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer);


/* Peek for the about-to-be-read character in the rxRingBuffer without incrementing the tail */
uint8_t xPeek(UARTRingBufferHandle_t *pxUARTRingBuffer, unsigned char *c);

/* Clears the entire rxRingBuffer and head/tail index */
void vFlushRXUART(UARTRingBufferHandle_t *pxUARTRingBuffer);


/* To be called during UART ISR */
void vISRUART(UART_HandleTypeDef *huart);



#endif /* INC_UARTRINGBUFFERGENERAL_H_ */
