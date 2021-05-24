/*
 * UART_DMA_IDLE.h
 *
 *  Created on: May 23, 2021
 *      Author: victor
 */

#ifndef INC_UART_DMA_IDLE_H_
#define INC_UART_DMA_IDLE_H_

#include <stdio.h>
#include <string.h>

/* APPLICATION PROGRAMMER */
#include "stm32l4xx_hal.h"

// UART2
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
void USER_UART2_IRQHandler(void);
void USER_UART2_IDLECallback(void);
// extern USER_UART_IRQHandler(UART_HandleTypeDef *huart); // stm_it


/* IMPLEMENTATION */
typedef struct
{
	uint8_t *puDMABuffer;
#if UART_BUFFER_SIZE <= 255UL
	uint8_t uDMABufferSize;
	uint8_t uHeadIndex;
	uint8_t uTailIndex;
#elif UART_BUFFER_SIZE <= 65535UL
	uint16_t uDMABufferSize;
	uint16_t uHeadIndex;
	uint16_t uTailIndex;
#else
	// Should not accept any DMA buffer larger than 65535UL items anyway
	uint16_t uDMABufferSize;
	uint16_t uHeadIndex;
	uint16_t uTailIndex;
#endif
	volatile uint8_t uRollOver;
} DMARingBufferHandle_t;


typedef struct
{
	UART_HandleTypeDef *huart;
	DMARingBufferHandle_t xRXBuffer;
	DMARingBufferHandle_t xTXBuffer;
} UARTRingBufferHandle_t;


/* Initialize ring buffer */
void vInitUARTRingBuffer(UARTRingBufferHandle_t *pxUARTRingBuffer,
		UART_HandleTypeDef *huart,
		uint8_t *dmaRX, uint32_t dmaRXSize,
		uint8_t *dmaTX, uint32_t dmaTXSize);


#endif /* INC_UART_DMA_IDLE_H_ */
