/*
 * UARTRingBuffer.c
 *
 *  Created on: May 22, 2021
 *      Author: victor
 */


#include "UARTRingBuffer.h"
#include <string.h>


/* ISR */
// extern void Uart_isr (UART_HandleTypeDef *huart);


/* IMPLEMENTATION */
void vInitUARTRingBuffer(void)
{
	pxRXRingBuffer = &rxRingBuffer;
	pxTXRingBuffer = &txRingBuffer;

	/* Enable interrupts */
	__HAL_UART_ENABLE_IT(UART_HANDLE, UART_IT_ERR);
	__HAL_UART_ENABLE_IT(UART_HANDLE, UART_IT_RXNE);
}


void vPutCharRXBuffer(unsigned char c)
{
	/* Get the index of the would-be next head. */
	uint32_t uNextHeadIndex = (uint32_t)((pxRXRingBuffer->uHeadIndex + 1) % UART_BUFFER_SIZE);

	/* Only put the character if the head does not overtake the tail within the ring.
	 * Not equal assumes would be effectively less than (even if the uNextHeadIndex > uTailIndex due to wrap around).
	 * So use condition: != instead of <=.
	 */
	if (uNextHeadIndex != pxRXRingBuffer->uTailIndex)
	{
		pxRingBuffer->uHeadIndex = uNextHeadIndex;
		pxRingBuffer->puBuffer[pxRingBuffer->uHeadIndex] = c;
	}
}


void vPutCharTXBuffer(unsigned char c)
{
	uint32_t uNextHeadIndex = (uint32_t)((pxTXRingerBuffer->uHeadIndex + 1) % UART_BUFFER_SIZE);

	/* Only put the character if the head does not overtake the tail within the ring.
	 * Not equal assumes would be effectively less than (even if the uNextHeadIndex > uTailIndex due to wrap around).
	 * So use condition: != instead of <=.
	 */
	if (uNextHeadIndex != pxTXRingBuffer->uTailIndex)
	{
		pxTXRingBuffer->uHeadIndex = uNextHeadIndex;
		pxTXRingBuffer[pxTXRingBuffer->uHeadIndex] = c;
	}
}


unsigned char xReadUART(void)
{
	/* Check if there is unprocessed/new data available (read/tail has not caught up to write/head)
	 * Return -1 to signify no new data available */
	if (pxRXRingBuffer->uHeadIndex == pxRXRingBuffer->uTailIndex) return '\0';

	/* Unprocessed/New data is available.
	 * Return it and increment the tailIndex to signify more room to put data into the rxRingBuffer */
	unsigned char c = pxRXRingBuffer->puBuffer[pxRXRingBuffer->uTailIndex];
	pxRXRingBuffer->uTailIndex = (pxRXRingBuffer->uTailIndex + 1) % UART_BUFFER_SIZE;
	return c;
}


uint8_t xWriteUART(unsigned char c)
{
	/* Don't write if c is a null character */
	if (c == '\0') return 0;

	uint32_t uNextHeadIndex = (pxTXRingBuffer->uHeadIndex + 1) % UART_BUFFER_SIZE;
	// while (uNextHeadIndex == pxTXRingBuffer->uTailIndex); // Block wait for

	/* If the txRingBuffer is full/has no room for put new data, then do not put data. Return 0 to signify no new data put
	 * Data is lost! If string is sent, may not get \r\n!
	 * So ensure that the buffer is:
	 * 	(1) Large enough
	 * 	(2) Is transmitted through UART often enough as for essentially head > tail to imply data can be put in
	 */
	if (uNextHeadIndex == pxTXRingBuffer->uTailIndex) return 0;

	/* The txRingBuffer had enough data, so write */
	pxTXRingBuffer->puBuffer[pxTXRingBuffer->uHeadIndex] = c;
	pxTXRingBuffer->uHeadIndex = uNextHeadIndex;

	/* Enable UART transmission interrupt */
	__HAL_UART_ENABLE_IT(UART_HANDLE, UART_IT_TXE);

	/* Return 1 to signify successfully put new data into txRingBuffer */
	return 1;
}




