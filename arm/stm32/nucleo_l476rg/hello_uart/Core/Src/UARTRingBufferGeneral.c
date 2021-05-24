/*
 * UARTRingBufferGeneral.c
 *
 *  Created on: May 22, 2021
 *      Author: victor
 */



#include "UARTRingBufferGeneral.h"
#include <string.h>


void vInitUARTRingBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer, UART_HandleTypeDef *uartHandle);

/* APPLICATION PROGRAMMER */
UARTRingBufferHandle_t uartRingBuffer1;
UARTRingBufferHandle_t uartRingBuffer2;


void vInitUARTRingBuffers(void)
{
	vInitUARTRingBuffer(&uartRingBuffer1, uart1);
	vInitUARTRingBuffer(&uartRingBuffer2, uart2);
}

void vISRUART(UART_HandleTypeDef *huart)
{
	uint32_t rISRFlags = READ_REG(huart->Instance->ISR); // SR
	uint32_t rCR1ITS = READ_REG(huart->Instance->CR1);

	/* Receive register not empty, so read from it and put it into the head of the rxRingBuffer */
	if (((rISRFlags & USART_ISR_RXNE) != RESET) && ((rCR1ITS & USART_CR1_RXNEIE) != RESET))
	{
		huart->Instance->ISR;
		unsigned char c = huart->Instance->RDR;

		if (huart == uart1)
		{
			vPutCharRXBuffer(&uartRingBuffer1, c);
		}
		else if (huart == uart2)
		{
			vPutCharRXBuffer(&uartRingBuffer2, c);
		}
	}

	/* Transmit register empty, so write to it and put it from the tail of the rxRingBuffer */
	if (((rISRFlags & USART_ISR_TXE) != RESET) && ((rCR1ITS & USART_CR1_TXEIE) != RESET))
	{
		if (huart == uart1)
		{
			if (uartRingBuffer1.xTXRingBuffer.uHeadIndex == uartRingBuffer1.xTXRingBuffer.uTailIndex)
			{
				__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
			}
			else
			{
				unsigned char c = uartRingBuffer1.xTXRingBuffer.puBuffer[uartRingBuffer1.xTXRingBuffer.uTailIndex];
				uartRingBuffer1.xTXRingBuffer.uTailIndex = (uartRingBuffer1.xTXRingBuffer.uTailIndex + 1) % UART_BUFFER_SIZE;

				huart->Instance->ISR;
				huart->Instance->TDR = c;
			}
		}
		else if (huart == uart2)
		{
			if (uartRingBuffer2.xTXRingBuffer.uHeadIndex == uartRingBuffer2.xTXRingBuffer.uTailIndex)
			{
				__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
			}
			else
			{
				unsigned char c = uartRingBuffer2.xTXRingBuffer.puBuffer[uartRingBuffer2.xTXRingBuffer.uTailIndex];
				uartRingBuffer2.xTXRingBuffer.uTailIndex = (uartRingBuffer2.xTXRingBuffer.uTailIndex + 1) % UART_BUFFER_SIZE;

				huart->Instance->ISR;
				huart->Instance->TDR = c;
			}
		}
	}
}

/* ISR */
// extern void vISRUART (UART_HandleTypeDef *huart);



/* IMPLEMENTATION */
void vInitUARTRingBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer, UART_HandleTypeDef *uartHandle)
{
	/* Initialize ring buffers */
	pxUARTRingBuffer->pxUARTHandle = uartHandle;
	memset(pxUARTRingBuffer->xRXRingBuffer.puBuffer, '\0', UART_BUFFER_SIZE);
	pxUARTRingBuffer->xRXRingBuffer.uHeadIndex = 0;
	pxUARTRingBuffer->xRXRingBuffer.uTailIndex = 0;
	memset(pxUARTRingBuffer->xTXRingBuffer.puBuffer, '\0', UART_BUFFER_SIZE);
	pxUARTRingBuffer->xTXRingBuffer.uHeadIndex = 0;
	pxUARTRingBuffer->xTXRingBuffer.uTailIndex = 0;

	/* Enable interrupts */
	__HAL_UART_ENABLE_IT(pxUARTRingBuffer->pxUARTHandle, UART_IT_ERR);
	__HAL_UART_ENABLE_IT(pxUARTRingBuffer->pxUARTHandle, UART_IT_RXNE);
}



void vPutCharRXBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer, unsigned char c)
{
	/* Get the index of the would-be next head. */
	uint32_t uNextHeadIndex = (uint32_t)((pxUARTRingBuffer->xRXRingBuffer.uHeadIndex + 1) % UART_BUFFER_SIZE);

	/* Only put the character if the head does not overtake the tail within the ring.
	 * Not equal assumes would be effectively less than (even if the uNextHeadIndex > uTailIndex due to wrap around).
	 * So use condition: != instead of <=.
	 */
	if (uNextHeadIndex != pxUARTRingBuffer->xRXRingBuffer.uTailIndex)
	{
		pxUARTRingBuffer->xRXRingBuffer.uHeadIndex = uNextHeadIndex;
		pxUARTRingBuffer->xRXRingBuffer.puBuffer[pxUARTRingBuffer->xRXRingBuffer.uHeadIndex] = c;
	}
}


void vPutCharTXBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer, unsigned char c)
{
	uint32_t uNextHeadIndex = (uint32_t)((pxUARTRingBuffer->xTXRingBuffer.uHeadIndex + 1) % UART_BUFFER_SIZE);

	/* Only put the character if the head does not overtake the tail within the ring.
	 * Not equal assumes would be effectively less than (even if the uNextHeadIndex > uTailIndex due to wrap around).
	 * So use condition: != instead of <=.
	 */
	if (uNextHeadIndex != pxUARTRingBuffer->xTXRingBuffer.uTailIndex)
	{
		pxUARTRingBuffer->xTXRingBuffer.uHeadIndex = uNextHeadIndex;
		pxUARTRingBuffer->xTXRingBuffer.puBuffer[pxUARTRingBuffer->xTXRingBuffer.uHeadIndex] = c;
	}
}


uint8_t xReadUART(UARTRingBufferHandle_t *pxUARTRingBuffer, unsigned char *c)
{
	/* Check if there is unprocessed/new data available (read/tail has not caught up to write/head)
	 * Return 0 to signify no new data available */
	if (pxUARTRingBuffer->xRXRingBuffer.uHeadIndex == pxUARTRingBuffer->xRXRingBuffer.uTailIndex) return 0;

	/* Unprocessed/New data is available.
	 * Return it and increment the tailIndex to signify more room to put data into the rxRingBuffer */
	unsigned char readC = pxUARTRingBuffer->xRXRingBuffer.puBuffer[pxUARTRingBuffer->xRXRingBuffer.uTailIndex];
	pxUARTRingBuffer->xRXRingBuffer.uTailIndex = (pxUARTRingBuffer->xRXRingBuffer.uTailIndex + 1) % UART_BUFFER_SIZE;
	*c = readC;

	/* Return 0 to signify data available and read from */
	return 1;
}


uint8_t xWriteUART(UARTRingBufferHandle_t *pxUARTRingBuffer, unsigned char c)
{
	/* Don't write if c is a null character */
	if (c == '\0') return 0;

	uint32_t uNextHeadIndex = (pxUARTRingBuffer->xTXRingBuffer.uHeadIndex + 1) % UART_BUFFER_SIZE;
	// while (uNextHeadIndex == pxTXRingBuffer->uTailIndex); // Block wait for

	/* If the txRingBuffer is full/has no room for put new data, then do not put data. Return 0 to signify no new data put
	 * Data is lost! If string is sent, may not get \r\n!
	 * So ensure that the buffer is:
	 * 	(1) Large enough
	 * 	(2) Is transmitted through UART often enough as for essentially head > tail to imply data can be put in
	 */
	if (uNextHeadIndex == pxUARTRingBuffer->xTXRingBuffer.uTailIndex) return 0;

	/* The txRingBuffer had enough data, so write */
	pxUARTRingBuffer->xTXRingBuffer.puBuffer[pxUARTRingBuffer->xTXRingBuffer.uHeadIndex] = c;
	pxUARTRingBuffer->xTXRingBuffer.uHeadIndex = uNextHeadIndex;

	/* Enable UART transmission interrupt */
	__HAL_UART_ENABLE_IT(pxUARTRingBuffer->pxUARTHandle, UART_IT_TXE);

	/* Return 1 to signify successfully put new data into txRingBuffer */
	return 1;
}


uint8_t xWriteStringUART(UARTRingBufferHandle_t *pxUARTRingBuffer, const unsigned char *s)
{
	/* Write each character of the string to the txRingBuffer until null-terminated */
	if (*s == '\0') return 0;
	while(*s != '\0') xWriteUART(pxUARTRingBuffer, *(s++));
	return 1;
}


uint32_t uGetNumReadableCharRXBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer)
{
	/* Adds UART_BUFFER_SIZE to account for uHeadIndex < uTailIndex. Will be modded in the end anyway if uHeadIndex >= uTailIndex */
	return (uint32_t)(((pxUARTRingBuffer->xRXRingBuffer.uHeadIndex - pxUARTRingBuffer->xRXRingBuffer.uTailIndex) + UART_BUFFER_SIZE ) % UART_BUFFER_SIZE);
}


uint8_t xPeek(UARTRingBufferHandle_t *pxUARTRingBuffer, unsigned char *c)
{
	if (pxUARTRingBuffer->xRXRingBuffer.uHeadIndex == pxUARTRingBuffer->xRXRingBuffer.uTailIndex) return 0;

	*c = pxUARTRingBuffer->xRXRingBuffer.puBuffer[pxUARTRingBuffer->xRXRingBuffer.uTailIndex];
	return 1;
}


void vFlushUART(UARTRingBufferHandle_t *pxUARTRingBuffer)
{
	memset(pxUARTRingBuffer->xRXRingBuffer.puBuffer, '\0', UART_BUFFER_SIZE);
	pxUARTRingBuffer->xRXRingBuffer.uHeadIndex = 0;
	pxUARTRingBuffer->xRXRingBuffer.uTailIndex = 0;
}




