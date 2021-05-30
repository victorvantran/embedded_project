/*
 * UART_DMA_IDLE.c
 *
 *  Created on: May 23, 2021
 *      Author: victor
 */

#include "UART_DMA_IDLE.h"

/* APPLICATION PROGRAMMER */
/*
 * Note: The delay of implicit CF when sending serial data through putty may cause idle interrupt to occur before \r is sent!
 * This will cause two idle interrupts to occur for say, "message\r" 1) message 2) \r.
 * To avoid this, explicitly put CR "message^M"
 * This will cause a subsequent idle of implicit CR when pressing enter, but that can be handled as its own "NO COMMAND RECEIVED"
 * for debugging purposes
 */


// UART2
UARTRingBufferHandle_t xUART2RingBuffer;

void USER_UART2_IRQHandler(void)
{
	if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);

		USER_UART_IDLECallback(&xUART2RingBuffer);
	}
}




/* IMPLEMENTATION */
void vInitUARTRingBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer,
		UART_HandleTypeDef *huart,
		uint8_t *dmaRX, uint32_t dmaRXSize,
		uint8_t *dmaTX, uint32_t dmaTXSize)
{
	// Structure
	pxUARTRingBuffer->huart = huart;
	pxUARTRingBuffer->xRXBuffer.puDMABuffer = dmaRX;
	pxUARTRingBuffer->xRXBuffer.uDMABufferSize = dmaRXSize;
	pxUARTRingBuffer->xRXBuffer.uHeadIndex = 0;
	pxUARTRingBuffer->xRXBuffer.uTailIndex = 0;
	pxUARTRingBuffer->xRXBuffer.uRollOver = 0;
	pxUARTRingBuffer->xTXBuffer.puDMABuffer = dmaTX;
	pxUARTRingBuffer->xTXBuffer.uDMABufferSize = dmaTXSize;
	pxUARTRingBuffer->xTXBuffer.uHeadIndex = 0;
	pxUARTRingBuffer->xTXBuffer.uTailIndex = 0;
	pxUARTRingBuffer->xTXBuffer.uRollOver = 0;

	// Receive DMA Buffer
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  HAL_UART_Receive_DMA(huart, pxUARTRingBuffer->xRXBuffer.puDMABuffer, pxUARTRingBuffer->xRXBuffer.uDMABufferSize);

  // Transfer DMA Buffer
}


void USER_UART_IDLECallback(UARTRingBufferHandle_t *pxUARTRingBuffer)
{
	// Tail catch up to head
	pxUARTRingBuffer->xRXBuffer.uHeadIndex = pxUARTRingBuffer->xRXBuffer.uDMABufferSize - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);

	// Task notification...
	uint16_t uTailIndex = pxUARTRingBuffer->xRXBuffer.uTailIndex;
	uint16_t uHeadIndex = pxUARTRingBuffer->xRXBuffer.uHeadIndex;
	uint8_t uRollOver = pxUARTRingBuffer->xRXBuffer.uRollOver;
	uint16_t uParseIndex = uTailIndex;

	if (uRollOver == 0)
	{
		while (uParseIndex != uHeadIndex)
		{
			if (pxUARTRingBuffer->xRXBuffer.puDMABuffer[uParseIndex] == '\r')
			{
				if (uParseIndex - uTailIndex > 0)
				{
					char *candidate = (char *)pxUARTRingBuffer->xRXBuffer.puDMABuffer + uTailIndex;
					size_t candidateLength = uParseIndex - uTailIndex;

					vHandleCandidateCommand(candidate, candidateLength);
				}

				// Candidate command found, so update tail to the start of next command in line
				uTailIndex = (uParseIndex + 1) % pxUARTRingBuffer->xRXBuffer.uDMABufferSize;
				pxUARTRingBuffer->xRXBuffer.uTailIndex = uTailIndex;
			}
			uParseIndex++;
		}
	}
	else if (uRollOver == 1)
	{
		if (uParseIndex > uHeadIndex)
		{
			while (uParseIndex < pxUARTRingBuffer->xRXBuffer.uDMABufferSize)
			{
				if (pxUARTRingBuffer->xRXBuffer.puDMABuffer[uParseIndex] == '\r')
				{
					if (uParseIndex - uTailIndex > 0)
					{
						char *candidate = (char *)pxUARTRingBuffer->xRXBuffer.puDMABuffer + uTailIndex;
						size_t candidateLength = uParseIndex - uTailIndex;

						vHandleCandidateCommand(candidate, candidateLength);
					}

					// Candidate command found, so update tail to the start of next command in line
					uTailIndex = (uParseIndex + 1) % pxUARTRingBuffer->xRXBuffer.uDMABufferSize;
					pxUARTRingBuffer->xRXBuffer.uTailIndex = uTailIndex;
				}
				uParseIndex++;
			}

			uParseIndex = 0;

			// Look for the next one to complete the firsthalf or just keep going
			while (uParseIndex != uHeadIndex)
			{
				if (pxUARTRingBuffer->xRXBuffer.puDMABuffer[uParseIndex] == '\r')
				{
					// if uTailIndex > uHeadIndex, use buffer, else use regular
					if (uTailIndex > uHeadIndex)
					{
						// uParseIndex will be less than tialIndex in this wrap-around case. So as long as they don't equal each other, a command was received
						if (uParseIndex - uTailIndex != 0)
						{
							char *candidateFirst = (char *)(pxUARTRingBuffer->xRXBuffer.puDMABuffer + uTailIndex);
							size_t candidateFirstLength = pxUARTRingBuffer->xRXBuffer.uDMABufferSize - uTailIndex;
							char *candidateSecond = (char *)(pxUARTRingBuffer->xRXBuffer.puDMABuffer);
							size_t candidateSecondLength = uParseIndex;

							vHandleCandidateCommandSplit(candidateFirst, candidateFirstLength, candidateSecond, candidateSecondLength);

							// Only unroll if tail has been successfully used for a wrap-around
							pxUARTRingBuffer->xRXBuffer.uRollOver = 0;
						}
					}
					// Wraparound found, so treat this as a regular, business as usual
					else
					{
						if (uParseIndex - uTailIndex > 0)
						{
							char *candidate = (char *)pxUARTRingBuffer->xRXBuffer.puDMABuffer + uTailIndex;
							size_t candidateLength = uParseIndex - uTailIndex;

							vHandleCandidateCommand(candidate, candidateLength);
						}
					}

					// Candidate command found, so update tail to the start of next command in line
					uTailIndex = (uParseIndex + 1) % pxUARTRingBuffer->xRXBuffer.uDMABufferSize;
					pxUARTRingBuffer->xRXBuffer.uTailIndex = uTailIndex;
				}

				uParseIndex++;
			}
		}
		else
		{
			// Reset due to too overflow rx buffer due to too much data received before it could all process
			HAL_UART_DMAStop(pxUARTRingBuffer->huart);
			vInitUARTRingBuffer(pxUARTRingBuffer, pxUARTRingBuffer->huart, pxUARTRingBuffer->xRXBuffer.puDMABuffer, pxUARTRingBuffer->xRXBuffer.uDMABufferSize,
					pxUARTRingBuffer->xTXBuffer.puDMABuffer, pxUARTRingBuffer->xTXBuffer.uDMABufferSize);
		}
	}
	else
	{
		// Reset due to too overflow rx buffer due to too much data received before it could all process
		HAL_UART_DMAStop(pxUARTRingBuffer->huart);
		vInitUARTRingBuffer(pxUARTRingBuffer, pxUARTRingBuffer->huart, pxUARTRingBuffer->xRXBuffer.puDMABuffer, pxUARTRingBuffer->xRXBuffer.uDMABufferSize,
				pxUARTRingBuffer->xTXBuffer.puDMABuffer, pxUARTRingBuffer->xTXBuffer.uDMABufferSize);
	}

	printf("TailIndex: %u, HeadIndex: %u\r\n", pxUARTRingBuffer->xRXBuffer.uTailIndex, pxUARTRingBuffer->xRXBuffer.uHeadIndex);

}



uint8_t bCommandMatch(const char *command, const char *candidate, size_t candidateLength)
{
	return (strlen(command) == candidateLength) && (strncmp(command, candidate, candidateLength) == 0);
}


uint8_t bCommandSplitMatch(const char *command,
		const char *candidateFirst, size_t candidateFirstLength,
		const char *candidateSecond, size_t candidateSecondLength)
{
	return (strlen(command) == candidateFirstLength + candidateSecondLength) &&
			(strncmp(command, candidateFirst, candidateFirstLength) == 0) &&
			(strncmp(command + candidateFirstLength, candidateSecond, candidateSecondLength) == 0);
}


void vHandleCandidateCommand(const char *candidate, size_t candidateLength)
{
	if (bCommandMatch("ON", candidate, candidateLength))
	{
		printf("SET LIGHT\r\n");
	}
	else if (bCommandMatch("OFF", candidate, candidateLength))
	{
		printf("UNSET LIGHT\r\n");
	}
	else
	{
		printf("INVLD\r\n");
	}
}


void vHandleCandidateCommandSplit(const char *candidateFirst, size_t candidateFirstLength,
		const char *candidateSecond, size_t candidateSecondLength)
{
	if (bCommandSplitMatch("ON", candidateFirst, candidateFirstLength, candidateSecond, candidateSecondLength))
	{
		printf("SET LIGHT\r\n");
	}
	else if (bCommandSplitMatch("OFF", candidateFirst, candidateFirstLength, candidateSecond, candidateSecondLength))
	{
		printf("UNSET LIGHT\r\n");
	}
	else
	{
		printf("INVLDsplit\r\n");
	}
}


uint8_t bTransmitCommand(UARTRingBufferHandle_t *xRingBuffer, const char *command, size_t numElements)
{
	/* Could also add a wait for a semaphore, and semaphore released from isr on transmit complete callback */
	strncpy(xRingBuffer->xTXBuffer.puDMABuffer, command, numElements);
	if (HAL_UART_Transmit_DMA(xRingBuffer->huart, (uint8_t *)xRingBuffer->xTXBuffer.puDMABuffer, numElements) == HAL_OK)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



