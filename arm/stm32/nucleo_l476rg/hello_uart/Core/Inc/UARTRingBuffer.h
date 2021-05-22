/*
 * UARTRingBuffer.h
 *
 *  Created on: May 22, 2021
 *      Author: victor
 */

#ifndef INC_UARTRINGBUFFER_H_
#define INC_UARTRINGBUFFER_H_

/* APPLICATION PROGRAMMER */
#include "stm32l4xx_hal.h"
UART_HandleTypeDef huart2;
#define UART_HANDLE &huart2;
#define UART_BUFFER_SIZE 256UL



/* IMPLEMENTATION */
typedef struct
{
	unsigned char puBuffer[UART_BUFFER_SIZE];
#if UART_BUFFER_SIZE <= 256UL
		volatile uint8_t uHeadIndex;
		volatile uint8_t uTailIndex;
#else if UART_BUFFER_SIZE <= 65536UL
		volatile uint16_t uHeadIndex;
		volatile uint16_t uTailIndex;
#else
		volatile uint32_t uHeadIndex;
		volatile uint32_t uTailIndex;
#endif
} RingBufferHandle_t;


RingBufferHandle_t rxRingBuffer = {{0}, 0, 0};
RingBufferHandle_t txRingBuffer = {{0}, 0, 0};

RingBufferHandle_t *pxRXRingBuffer;
RingBufferHandle_t *pxTXRingBuffer;



/* Initialize ring buffer */
void vInitUARTRingBuffer(void);


/* */
void vPutCharRXBuffer(unsigned char c);

/* */
void vPutCharTXBuffer(unsigned char c);


/* Processes the incoming unsigned char data into the rxRingBuffer and increment that tailIndex of said buffer
 * Returns null char if head == tail, which implies there is no unprocessed/new data in the rxRingBuffer yet.
 * */
unsigned char xReadUART(void);

/* Write the incoming unsigned char data into the txRingBuffer and increment that tailIndex of said buffer
 * Returns 0 if head == tail, which implies unprocessed/new data filled the entire txRingBuffer.
 * Thus, the txRingBuffer refuses to overwrite/put data in this implementation.
 * Returns 0 for failure write and returns 0 for successful write.
 * */
uint8_t xWriteUART(unsigned char c);







/* Reads the incoming unsigned char data into the rxRingBuffer and increment that tailIndex of said buffer
 * Returns -1 if head == tail, which implies unprocessed/new data filled the entire rxRingBuffer.
 * Thus, the rxRingBuffer refuses to overwrite data in this implementation.
 * */


#endif /* INC_UARTRINGBUFFER_H_ */
