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


/* IMPLEMENTATION */





/* APPLICATION PROGRAMMER */
#include "stm32l4xx_hal.h"

// UART2
#define UART_BUFFER_SIZE 128UL
void USER_ThingSpeak_IRQHandler(UART_HandleTypeDef *pxHUART);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *pxHUART);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *pxHUART);


/* IMPLEMENTATION */
typedef struct
{
	uint8_t puDMABuffer[UART_BUFFER_SIZE];
#if UART_BUFFER_SIZE <= 255UL
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
	DMA_HandleTypeDef *pxUART_DMA_RX;
	DMARingBufferHandle_t xRXBuffer;
	DMARingBufferHandle_t xTXBuffer;
} ThingSpeakHandle_t;


/* Initialize ring buffer */
void vInitThingSpeak(ThingSpeakHandle_t *pxThingSpeak, UART_HandleTypeDef *huart, DMA_HandleTypeDef *pxUART_DMA_RX);


/* Receive */
void USER_UART_IDLECallback(ThingSpeakHandle_t *pxThingSpeak);

uint8_t bCommandMatch(const char *command, const char *candidate, size_t commandLength);
uint8_t bCommandSplitMatch(const char *command,
		const char *candidateFirst, size_t candidateFirstLength,
		const char *candidateSecond, size_t candidateSecondLength);

void vHandleCandidateCommand(const char *candidate, size_t candidateLength);
void vHandleCandidateCommandSplit(const char *candidateFirst, size_t candidateFirstLength,
		const char *candidateSecond, size_t candidateSecondLength);


/* Transmit */
uint8_t bTransmitCommand(ThingSpeakHandle_t *pxThingSpeak, const char *command, size_t numElements);



#endif /* INC_THINGSPEAK_H_ */
