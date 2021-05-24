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

	xUART2RingBuffer.xRXBuffer.uTailIndex = xUART2RingBuffer.xRXBuffer.uDMABufferSize - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	printf("Head: %u, TailIndex: %u\r\n", xUART2RingBuffer.xRXBuffer.uHeadIndex, xUART2RingBuffer.xRXBuffer.uTailIndex);

	xUART2RingBuffer.xRXBuffer.uHeadIndex = xUART2RingBuffer.xRXBuffer.uTailIndex;
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
	pxUARTRingBuffer->xTXBuffer.puDMABuffer = dmaTX;
	pxUARTRingBuffer->xTXBuffer.uDMABufferSize = dmaTXSize;
	pxUARTRingBuffer->xTXBuffer.uHeadIndex = 0;
	pxUARTRingBuffer->xTXBuffer.uTailIndex = 0;

	// Receive DMA Buffer
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  HAL_UART_Receive_DMA(huart, pxUARTRingBuffer->xRXBuffer.puDMABuffer, pxUARTRingBuffer->xRXBuffer.uDMABufferSize);

  // Transfer DMA Buffer

}

