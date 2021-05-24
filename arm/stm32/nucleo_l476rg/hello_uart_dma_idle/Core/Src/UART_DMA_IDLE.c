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

void USER_UART2_IDLECallback(void)
{
	//HAL_UART_DMAStop(huart);

	//uint8_t data_length = RX_BFR_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	//printf("Received Data(length = %d)\r\n", data_length);

	//HAL_UART_Transmit(huart, rx2Buffer, data_length, HAL_MAX_DELAY);

	//memset(rx2Buffer, 0, data_length);
	//data_length = 0;

	//HAL_UART_Receive_DMA(huart, rx2Buffer, sizeof(rx2Buffer));

	/*
	tailIndex = RX_BFR_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	printf("Head: %u, TailIndex: %u\r\n", headIndex, tailIndex);

	headIndex = tailIndex;
	*/

	/*
	 Head and tail backwards!
	xUART2RingBuffer.xRXBuffer.uTailIndex = xUART2RingBuffer.xRXBuffer.uDMABufferSize - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	printf("Head: %u, TailIndex: %u\r\n", xUART2RingBuffer.xRXBuffer.uHeadIndex, xUART2RingBuffer.xRXBuffer.uTailIndex);
	xUART2RingBuffer.xRXBuffer.uHeadIndex = xUART2RingBuffer.xRXBuffer.uTailIndex;
	*/




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
				// [!] size arguement of strncmp due to circular

				if (uParseIndex - uTailIndex > 0)
				{
					if (strncmp((char *)xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex, "ON", uParseIndex - uTailIndex) == 0)
					{
						printf("SET LIGHTz\r\n");
					}
					else if (strncmp((char *)xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex, "OFF", uParseIndex - uTailIndex) == 0)
					{
						printf("UNSET LIGHTz\r\n");
					}
					else
					{
						printf("INVALID COMMAND RECEIVED\r\n");
					}
				}
				else
				{
					printf("NO COMMAND RECEIVED (JUST CR)\r\n");
				}

				// Command found, so update tail to the start of next command in line
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
			char commandBuff[xUART2RingBuffer.xRXBuffer.uDMABufferSize];

			while (uParseIndex < xUART2RingBuffer.xRXBuffer.uDMABufferSize)
			{
				if (xUART2RingBuffer.xRXBuffer.puDMABuffer[uParseIndex] == '\r')
				{
					// [!] size arguement of strncmp due to circular

					if (uParseIndex - uTailIndex > 0)
					{
						if (strncmp((char *)xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex, "ON", uParseIndex - uTailIndex) == 0)
						{
							printf("SET LIGHTpr\r\n");
						}
						else if (strncmp((char *)xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex, "OFF", uParseIndex - uTailIndex) == 0)
						{
							printf("UNSET LIGHTpr\r\n");
						}
						else
						{
							printf("INVALID COMMAND RECEIVED\r\n");
						}
					}
					else
					{
						printf("NO COMMAND RECEIVED (JUST CR)\r\n");
					}

					// Command found, so update tail to the start of next command in line
					uTailIndex = (uParseIndex + 1) % xUART2RingBuffer.xRXBuffer.uDMABufferSize;
					xUART2RingBuffer.xRXBuffer.uTailIndex = uTailIndex;
				}
				uParseIndex++;
			}


			uParseIndex = 0;
			xUART2RingBuffer.xRXBuffer.uRollOver = 0;
			uint16_t uFirstHalf = xUART2RingBuffer.xRXBuffer.uDMABufferSize - uTailIndex;
			memcpy(commandBuff, (char *)xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex, uFirstHalf);


			while (uParseIndex != uHeadIndex)
			{
				if (xUART2RingBuffer.xRXBuffer.puDMABuffer[uParseIndex] == '\r')
				{
					// if uTailIndex > uHeadIndex, use buffer, else use regular
					if (uTailIndex > uHeadIndex)
					{
						// Combine and compare
						memcpy(commandBuff + uFirstHalf, (char *)xUART2RingBuffer.xRXBuffer.puDMABuffer, uParseIndex);

						// uParseIndex will be less than tialIndex in this wrap-around case. So as long as they don't equal each other, a command was received
						if (uParseIndex - uTailIndex != 0)
						{
							if (strncmp(commandBuff, "ON", uFirstHalf + uParseIndex) == 0)
							{
								printf("SET LIGHTrin\r\n");
							}
							else if (strncmp(commandBuff, "OFF", uFirstHalf + uParseIndex) == 0)
							{
								printf("UNSET LIGHTrin\r\n");
							}
							else
							{
								printf("INVALID COMMAND RECEIVED\r\n");
							}
						}
						else
						{
							printf("NO COMMAND RECEIVED (JUST CR)zzz\r\n");
						}

					}
					else
					{
						if (uParseIndex - uTailIndex > 0)
						{
							if (strncmp((char *)xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex, "ON", uParseIndex - uTailIndex) == 0)
							{
								printf("SET LIGHTrf\r\n");
							}
							else if (strncmp((char *)xUART2RingBuffer.xRXBuffer.puDMABuffer + uTailIndex, "OFF", uParseIndex - uTailIndex) == 0)
							{
								printf("UNSET LIGHTrf\r\n");
							}
							else
							{
								printf("INVALID COMMAND RECEIVED\r\n");
							}
						}
						else
						{
							printf("NO COMMAND RECEIVED (JUST CR)\r\n");
						}
					}


					// Command found, so update tail to the start of next command in line
					uTailIndex = (uParseIndex + 1) % xUART2RingBuffer.xRXBuffer.uDMABufferSize;
					xUART2RingBuffer.xRXBuffer.uTailIndex = uTailIndex;
				}

				uParseIndex++;
			}

		}
		else
		{
			printf("TOO MUCH DATA SENT AT ONCE BEFORE IT CAN BE PROCESSED. TRY INCREASING BUFFER SIZE\r\n");
			xUART2RingBuffer.xRXBuffer.uTailIndex = uHeadIndex;
			xUART2RingBuffer.xRXBuffer.uRollOver = 0;
		}
	}
	else
	{
		printf("TOO MUCH DATA SENT AT ONCE BEFORE IT CAN BE PROCESSED. TRY INCREASING BUFFER SIZE\r\n");
		xUART2RingBuffer.xRXBuffer.uTailIndex = uHeadIndex;
		xUART2RingBuffer.xRXBuffer.uRollOver = 0;
	}

	printf("TailIndex: %u, HeadIndex: %u\r\n", xUART2RingBuffer.xRXBuffer.uTailIndex, xUART2RingBuffer.xRXBuffer.uHeadIndex);

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

