/*
 * thingspeak.h
 *
 *  Created on: May 30, 2021
 *      Author: victor
 */

#ifndef INC_THINGSPEAK_H_
#define INC_THINGSPEAK_H_


#include <stdio.h>
#include <string.h>

/* APPLICATION PROGRAMMER */
#include "stm32l4xx_hal.h"

// UART2
#define UART_BUFFER_SIZE 128UL

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
#error "DMA buffer should not be hold more than 65535 items"
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



uint8_t bCommandMatch(const char *command, const char *candidate, size_t commandLength);


uint8_t bCommandSplitMatch(const char *command,
		const char *candidateFirst, size_t candidateFirstLength,
		const char *candidateSecond, size_t candidateSecondLength);



void vHandleCandidateCommand(const char *candidate, size_t candidateLength);


void vHandleCandidateCommandSplit(const char *candidateFirst, size_t candidateFirstLength,
		const char *candidateSecond, size_t candidateSecondLength);


/* Transmit */
uint8_t bTransmitCommand(UARTRingBufferHandle_t *xRingBuffer, const char *command, size_t numElements);




#endif /* INC_THINGSPEAK_H_ */
