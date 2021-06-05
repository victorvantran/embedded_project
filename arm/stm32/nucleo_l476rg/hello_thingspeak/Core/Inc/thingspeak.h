/*
 * thingspeak.h
 *
 *  Created on: May 30, 2021
 *      Author: victor
 */

#ifndef INC_THINGSPEAK_H_
#define INC_THINGSPEAK_H_



/* APPLICATION PROGRAMMER */
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

#define UART_BUFFER_SIZE 512UL //256UL
// call USER_ThingSpeak_IRQHandler(UART_HandleTypeDef *pxHUART) in stm32xxxx_it.c

#define THINGSPEAK_TASK_NAME "thingSpeakTask"
#define THINGSPEAK_TASK_SIZE (128 * 4) + (UART_BUFFER_SIZE)
#define THINGSPEAK_TASK_PRIORITY (osPriority_t) osPriorityAboveNormal

#define PROCESS_MESSAGE_TASK_NAME "processMessageTask"
#define PROCESS_MESSAGE_TASK_SIZE (128 * 4) + (UART_BUFFER_SIZE)
#define PROCESS_MESSAGE_TASK_PRIORITY (osPriority_t) osPriorityNormal



/* IMPLEMENTATION */
#include <stdio.h>
#include <string.h>


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
	osThreadId_t xThingSpeakTaskHandle;
	osThreadAttr_t xThingSpeakTaskAttributes;
	osThreadId_t xProcMessageTaskHandle;
	osThreadAttr_t xProcMessageTaskAttributes;
} ThingSpeakHandle_t;


void vStartThingSpeakTask(void *argument);
void vStartProcMessageTask(void *argument);


/* Commands */
typedef enum
{
	OK,
	GT,
	SEND_OK,
	CLOSED,
	AT_ERROR

} ATCommand_t;


/* Initialize ring buffer */
void vInitThingSpeak(ThingSpeakHandle_t *pxThingSpeak, UART_HandleTypeDef *huart, DMA_HandleTypeDef *pxUART_DMA_RX,
		const char *pcThingSpeakTaskName, uint32_t uThingSpeakTaskSize, osPriority_t xThingSpeakTaskPriority,
		const char *pcProcessMessageTaskName, uint32_t uProcessMessageTaskSize, osPriority_t xProcessMessageTaskPriority);

void vRefreshThingSpeak(ThingSpeakHandle_t *pxThingSpeak);

/* Receive */
void USER_UART_IDLECallback(ThingSpeakHandle_t *pxThingSpeak);

/* AT Commands and Messages end in \r\n */
uint8_t bParseMessage(ThingSpeakHandle_t *pxThingSpeak, uint16_t uHeadIndex);
uint8_t bEndMatch(ThingSpeakHandle_t *pxThingSpeak, uint16_t uParseIndex);

uint8_t bCommandMatch(const char *command, const char *candidate, size_t commandLength);
uint8_t bCommandSplitMatch(const char *command,
		const char *candidateFirst, size_t candidateFirstLength,
		const char *candidateSecond, size_t candidateSecondLength);
uint8_t bCommandSubMatch(const char *command, const char *candidate, size_t commandLength);
uint8_t bCommandSplitSubMatch(const char *command,
		const char *candidateFirst, size_t candidateFirstLength,
		const char *candidateSecond, size_t candidateSecondLength);

void vHandleCandidateCommand(ThingSpeakHandle_t *pxThingSpeak, const char *candidate, size_t candidateLength);
void vHandleCandidateCommandSplit(ThingSpeakHandle_t *pxThingSpeak, const char *candidateFirst, size_t candidateFirstLength,
		const char *candidateSecond, size_t candidateSecondLength);

uint8_t bReceiveThingSpeakCommand(ThingSpeakHandle_t *pxThingSpeak, ATCommand_t eATCommand, TickType_t xTicksToWait);


/* Transmit */
uint8_t bTransmitCommand(ThingSpeakHandle_t *pxThingSpeak, const char *command, size_t numElements);


/* Event Handler */
void USER_ThingSpeak_IRQHandler(UART_HandleTypeDef *pxHUART);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *pxHUART);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *pxHUART);


/* ThingSpeak */
uint8_t bTransmitThingSpeakData(ThingSpeakHandle_t *pxThingSpeak, char *apiKey, uint8_t field, uint16_t value);

/* ThingSpeak */
uint8_t bTransmitElephantSQLData(ThingSpeakHandle_t *pxThingSpeak, char *apiKey, uint8_t field, uint16_t value);


#endif /* INC_THINGSPEAK_H_ */
