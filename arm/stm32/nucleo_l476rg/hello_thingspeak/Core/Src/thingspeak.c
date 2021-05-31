/*
 * thingspeak.c
 *
 *  Created on: May 30, 2021
 *      Author: victor
 */


#include "thingspeak.h"

/* APPLICATION PROGRAMMER */
/*
 * Note: The delay of implicit CF when sending serial data through putty may cause idle interrupt to occur before \r is sent!
 * This will cause two idle interrupts to occur for say, "message\r" 1) message 2) \r.
 * To avoid this, explicitly put CR "message^M"
 * This will cause a subsequent idle of implicit CR when pressing enter, but that can be handled as its own "NO COMMAND RECEIVED"
 * for debugging purposes
 */

/* extern ThingSpeakHandle_t xThingSpeak in main */



/* IMPLEMENTATION */
ThingSpeakHandle_t xThingSpeak;


void vInitThingSpeak(ThingSpeakHandle_t *pxThingSpeak, UART_HandleTypeDef *huart, DMA_HandleTypeDef *pxUART_DMA_RX)
{
	// Structure
	pxThingSpeak->huart = huart;
	pxThingSpeak->pxUART_DMA_RX = pxUART_DMA_RX;
	memset(pxThingSpeak->xRXBuffer.puDMABuffer, 0, sizeof(pxThingSpeak->xRXBuffer.puDMABuffer));
	pxThingSpeak->xRXBuffer.uHeadIndex = 0;
	pxThingSpeak->xRXBuffer.uTailIndex = 0;
	pxThingSpeak->xRXBuffer.uRollOver = 0;
	memset(pxThingSpeak->xTXBuffer.puDMABuffer, 0, sizeof(pxThingSpeak->xTXBuffer.puDMABuffer));
	pxThingSpeak->xTXBuffer.uHeadIndex = 0;
	pxThingSpeak->xTXBuffer.uTailIndex = 0;
	pxThingSpeak->xTXBuffer.uRollOver = 0;

	// Receive DMA Buffer
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  HAL_UART_Receive_DMA(huart, pxThingSpeak->xRXBuffer.puDMABuffer, sizeof(pxThingSpeak->xRXBuffer.puDMABuffer));
}


void USER_UART_IDLECallback(ThingSpeakHandle_t *pxThingSpeak)
{
	// Tail catch up to head
	pxThingSpeak->xRXBuffer.uHeadIndex = sizeof(pxThingSpeak->xRXBuffer.puDMABuffer) - __HAL_DMA_GET_COUNTER(pxThingSpeak->pxUART_DMA_RX);

	// [!] To be Task deferred...
	uint16_t uTailIndex = pxThingSpeak->xRXBuffer.uTailIndex;
	uint16_t uHeadIndex = pxThingSpeak->xRXBuffer.uHeadIndex;
	uint8_t uRollOver = pxThingSpeak->xRXBuffer.uRollOver;
	uint16_t uParseIndex = uTailIndex;

	if (uRollOver == 0)
	{
		while (uParseIndex != uHeadIndex)
		{
			//if (pxThingSpeak->xRXBuffer.puDMABuffer[uParseIndex] == '\r')
			if (bEndMatch(pxThingSpeak, uParseIndex))
			{
				if (uParseIndex - uTailIndex > 0)
				{
					char *candidate = (char *)pxThingSpeak->xRXBuffer.puDMABuffer + uTailIndex;
					size_t candidateLength = uParseIndex - uTailIndex - 1;

					vHandleCandidateCommand(candidate, candidateLength);
				}

				// Candidate command found, so update tail to the start of next command in line
				uTailIndex = (uParseIndex + 1) % sizeof(pxThingSpeak->xRXBuffer.puDMABuffer) ;
				pxThingSpeak->xRXBuffer.uTailIndex = uTailIndex;
			}
			uParseIndex++;
		}
	}
	else if (uRollOver == 1)
	{
		if (uParseIndex > uHeadIndex)
		{
			while (uParseIndex < sizeof(pxThingSpeak->xRXBuffer.puDMABuffer) )
			{
				if (bEndMatch(pxThingSpeak, uParseIndex))
				{
					if (uParseIndex - uTailIndex > 0)
					{
						char *candidate = (char *)pxThingSpeak->xRXBuffer.puDMABuffer + uTailIndex;
						size_t candidateLength = uParseIndex - uTailIndex - 1;

						vHandleCandidateCommand(candidate, candidateLength);
					}

					// Candidate command found, so update tail to the start of next command in line
					uTailIndex = (uParseIndex + 1) % sizeof(pxThingSpeak->xRXBuffer.puDMABuffer);
					pxThingSpeak->xRXBuffer.uTailIndex = uTailIndex;
				}
				uParseIndex++;
			}

			uParseIndex = 0;

			// Look for the next one to complete the firsthalf or just keep going
			while (uParseIndex != uHeadIndex)
			{
				if (bEndMatch(pxThingSpeak, uParseIndex))
				{
					// if uTailIndex > uHeadIndex, use buffer, else use regular
					if (uTailIndex > uHeadIndex)
					{
						// uParseIndex will be less than tialIndex in this wrap-around case. So as long as they don't equal each other, a command was received
						if (uParseIndex - uTailIndex != 0)
						{
							char *candidateFirst = (char *)(pxThingSpeak->xRXBuffer.puDMABuffer + uTailIndex);
							size_t candidateFirstLength = sizeof(pxThingSpeak->xRXBuffer.puDMABuffer) - uTailIndex;
							char *candidateSecond = (char *)(pxThingSpeak->xRXBuffer.puDMABuffer);
							size_t candidateSecondLength = uParseIndex;

							vHandleCandidateCommandSplit(candidateFirst, candidateFirstLength, candidateSecond, candidateSecondLength);

							// Only unroll if tail has been successfully used for a wrap-around
							pxThingSpeak->xRXBuffer.uRollOver = 0;
						}
					}
					// Wraparound found, so treat this as a regular, business as usual
					else
					{
						if (uParseIndex - uTailIndex > 0)
						{
							char *candidate = (char *)pxThingSpeak->xRXBuffer.puDMABuffer + uTailIndex;
							size_t candidateLength = uParseIndex - uTailIndex - 1;

							vHandleCandidateCommand(candidate, candidateLength);
						}
					}

					// Candidate command found, so update tail to the start of next command in line
					uTailIndex = (uParseIndex + 1) % sizeof(pxThingSpeak->xRXBuffer.puDMABuffer);
					pxThingSpeak->xRXBuffer.uTailIndex = uTailIndex;
				}

				uParseIndex++;
			}
		}
		else
		{
			// Reset due to too overflow rx buffer due to too much data received before it could all process
			HAL_UART_DMAStop(pxThingSpeak->huart);
			vInitThingSpeak(pxThingSpeak, pxThingSpeak->huart, pxThingSpeak->pxUART_DMA_RX);
		}
	}
	else
	{
		// Reset due to too overflow rx buffer due to too much data received before it could all process
		HAL_UART_DMAStop(pxThingSpeak->huart);
		vInitThingSpeak(pxThingSpeak, pxThingSpeak->huart, pxThingSpeak->pxUART_DMA_RX);
	}

	printf("TailIndex: %u, HeadIndex: %u\r\n", pxThingSpeak->xRXBuffer.uTailIndex, pxThingSpeak->xRXBuffer.uHeadIndex);

}


uint8_t bEndMatch(ThingSpeakHandle_t *pxThingSpeak, uint16_t uParseIndex)
{
	return (pxThingSpeak->xRXBuffer.puDMABuffer[uParseIndex] == '\n') &&
			(pxThingSpeak->xRXBuffer.puDMABuffer[(uParseIndex - 1) % sizeof(pxThingSpeak->xRXBuffer.puDMABuffer)] == '\r');
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


/* Debug purposes on serial monitor */
extern UART_HandleTypeDef huart2;


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
	else if (bCommandMatch("OK", candidate, candidateLength))
	{
		printf("OK Received\r\n");
	}
	else
	{
		//HAL_UART_Transmit(&huart2, "INVLD: ", 7, 1000);
		HAL_UART_Transmit(&huart2, (char *)candidate, candidateLength, 1000);
		//printf("INVLD\r\n");
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
	else if (bCommandSplitMatch("OK", candidateFirst, candidateFirstLength, candidateSecond, candidateSecondLength))
	{
		printf("OK Received\r\n");
	}
	else
	{
		HAL_UART_Transmit(&huart2, (char *)candidateFirst, candidateFirstLength, 1000);
		HAL_UART_Transmit(&huart2, (char *)candidateSecond, candidateSecondLength, 1000);
		//printf("INVLD\r\n");
	}
}


uint8_t bTransmitCommand(ThingSpeakHandle_t *pxThingSpeak, const char *command, size_t numElements)
{
	/* [!] Could also add a wait for a semaphore, and semaphore released from isr on transmit complete callback */
	strncpy((char *)pxThingSpeak->xTXBuffer.puDMABuffer, command, numElements);
	if (HAL_UART_Transmit_DMA(pxThingSpeak->huart, (uint8_t *)pxThingSpeak->xTXBuffer.puDMABuffer, numElements) == HAL_OK)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void USER_ThingSpeak_IRQHandler(UART_HandleTypeDef *pxHUART)
{
	if (__HAL_UART_GET_FLAG(pxHUART, UART_FLAG_IDLE) != RESET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(pxHUART);

		USER_UART_IDLECallback(&xThingSpeak);
	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *pxHUART)
{
	if (pxHUART == xThingSpeak.huart)
	{
		xThingSpeak.xRXBuffer.uRollOver++;
	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *pxHUART)
{
	__NOP();
}



