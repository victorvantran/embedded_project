/*
 * UART_DMA_IDLE.c
 *
 *  Created on: May 23, 2021
 *      Author: victor
 */

#include "UART_DMA_IDLE.h"

/* APPLICATION PROGRAMMER */
// UART2
UARTRingBufferHandle_t xUART2RingBuffer;

void USER_UART2_IRQHandler(void)
{
	if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);

		printf("UART2 Idle IRQ Detected\r\n");
		USER_UART2_IDLECallback();
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


void USER_UART2_IDLECallback(void)
{
	// Tail catch up to head
	xUART2RingBuffer.xRXBuffer.uHeadIndex = xUART2RingBuffer.xRXBuffer.uDMABufferSize - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);

	// Task notification...
	uint16_t uTailIndex = xUART2RingBuffer.xRXBuffer.uTailIndex;
	uint16_t uHeadIndex = xUART2RingBuffer.xRXBuffer.uHeadIndex;
	uint16_t uParseIndex = uTailIndex;

	// Complete and half complete...
	uint8_t uRollOver = xUART2RingBuffer.xRXBuffer.uRollOver;
	if (uRollOver == 0)
	{
		while (uParseIndex != uHeadIndex)
		{
			if (xUART2RingBuffer.xRXBuffer.puDMABuffer[uParseIndex] == '\r')
			{
				if (uParseIndex - uTailIndex > 0)
				{
					char *candidate = (char *)xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex;
					size_t candidateLength = uParseIndex - uTailIndex;

					vHandleCandidateCommand(candidate, candidateLength);
				}
				else
				{
					printf("NO COMMAND RECEIVED (JUST CR)z\r\n");
				}

				// Candidate command found, so update tail to the start of next command in line
				uTailIndex = (uParseIndex + 1) % xUART2RingBuffer.xRXBuffer.uDMABufferSize;
				xUART2RingBuffer.xRXBuffer.uTailIndex = uTailIndex;
			}

			uParseIndex++;
		}
	}
	else if (uRollOver == 1)
	{
		if (uParseIndex > uHeadIndex)
		{
			while (uParseIndex < xUART2RingBuffer.xRXBuffer.uDMABufferSize)
			{
				if (xUART2RingBuffer.xRXBuffer.puDMABuffer[uParseIndex] == '\r')
				{
					// [!] size arguement of strncmp due to circular

					if (uParseIndex - uTailIndex > 0)
					{
						char *candidate = (char *)xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex;
						size_t candidateLength = uParseIndex - uTailIndex;
						vHandleCandidateCommand(candidate, candidateLength);
					}
					else
					{
						printf("NO COMMAND RECEIVED (JUST CR)pr\r\n");
					}

					// Candidate command found, so update tail to the start of next command in line
					uTailIndex = (uParseIndex + 1) % xUART2RingBuffer.xRXBuffer.uDMABufferSize;
					xUART2RingBuffer.xRXBuffer.uTailIndex = uTailIndex;
				}
				uParseIndex++;
			}

			uParseIndex = 0;
			xUART2RingBuffer.xRXBuffer.uRollOver = 0;

			// Look for the next one to complete the firsthalf or just keep going
			while (uParseIndex != uHeadIndex)
			{
				if (xUART2RingBuffer.xRXBuffer.puDMABuffer[uParseIndex] == '\r')
				{
					// if uTailIndex > uHeadIndex, use buffer, else use regular
					if (uTailIndex > uHeadIndex)
					{
						// uParseIndex will be less than tialIndex in this wrap-around case. So as long as they don't equal each other, a command was received
						if (uParseIndex - uTailIndex != 0)
						{
							char *candidateFirst = (char *)(xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex);
							size_t candidateFirstLength = xUART2RingBuffer.xRXBuffer.uDMABufferSize - uTailIndex;
							char *candidateSecond = (char *)(xUART2RingBuffer.xRXBuffer.puDMABuffer);
							size_t candidateSecondLength = uParseIndex;

							vHandleCandidateCommandSplit(candidateFirst, candidateFirstLength, candidateSecond, candidateSecondLength);
						}
						else
						{
							printf("NO COMMAND RECEIVED (JUST CR)in\r\n");
						}
					}
					// Wraparound found, so treat this as a regular, business as usual
					else
					{
						if (uParseIndex - uTailIndex > 0)
						{
							char *candidate = (char *)xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex;
							size_t candidateLength = uParseIndex - uTailIndex;

							vHandleCandidateCommand(candidate, candidateLength);
						}
						else
						{
							printf("NO COMMAND RECEIVED (JUST CR)rf\r\n");
						}
					}

					// Candidate command found, so update tail to the start of next command in line
					uTailIndex = (uParseIndex + 1) % xUART2RingBuffer.xRXBuffer.uDMABufferSize;
					xUART2RingBuffer.xRXBuffer.uTailIndex = uTailIndex;
				}

				uParseIndex++;
			}
		}
		else
		{
			printf("TOO MUCH DATA SENT AT ONCE BEFORE IT CAN BE PROCESSED. TRY INCREASING BUFFER SIZE >1\r\n");
			xUART2RingBuffer.xRXBuffer.uTailIndex = uHeadIndex;
			xUART2RingBuffer.xRXBuffer.uRollOver = 0;
		}
	}
	else
	{
		printf("TOO MUCH DATA SENT AT ONCE BEFORE IT CAN BE PROCESSED. TRY INCREASING BUFFER SIZE >2\r\n");
		xUART2RingBuffer.xRXBuffer.uTailIndex = uHeadIndex;
		xUART2RingBuffer.xRXBuffer.uRollOver = 0;
	}

	printf("TailIndex: %u, HeadIndex: %u\r\n", xUART2RingBuffer.xRXBuffer.uTailIndex, xUART2RingBuffer.xRXBuffer.uHeadIndex);

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
		printf("INVALID COMMAND RECEIVED\r\n");
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
		printf("INVALID COMMAND RECEIVED\r\n");
	}
}


