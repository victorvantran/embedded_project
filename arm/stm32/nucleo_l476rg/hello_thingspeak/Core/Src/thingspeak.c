/*
 * thingspeak.c
 *
 *  Created on: May 30, 2021
 *      Author: victor
 */


#include "thingspeak.h"

/* APPLICATION PROGRAMMER */
/*processMessageTask
 * Note: The delay of implicit CF when sending serial data through putty may cause idle interrupt to occur before \r is sent!
 * This will cause two idle interrupts to occur for say, "message\r" 1) message 2) \r.
 * To avoid this, explicitly put CR "message^M"
 * This will cause a subsequent idle of implicit CR when pressing enter, but that can be handled as its own "NO COMMAND RECEIVED"
 * for debugging purposes
 */


ThingSpeakHandle_t xThingSpeak;
/* extern ThingSpeakHandle_t xThingSpeak in main */

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




/* IMPLEMENTATION */
void vStartThingSpeakTask(void *argument)
{
	//bTransmitCommand(&xThingSpeak, "AT+RST\r\n", 4 + 4);
	//HAL_Delay(3000);
	ThingSpeakHandle_t *pxThingSpeak = argument;

	int i = 711;
  for(;;)
  {
  	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

  	//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

  	//printf("hello task\r\n");

  	//bTransmitCommand(&xThingSpeak, "AT+CWLAP\r\n", 4 + 6);
  	//bTransmitCommand(&xThingSpeak, "AT\r\n", 4);

  	//bTransmitCommand(&xThingSpeak, "AT+CIFSR\r\n", 4 + 6);

  	//bTransmitThingSpeakData(pxThingSpeak, "HE3ZUVZ1MKI1FOPB", 1, i);

  	bTransmitThingSpeakData(pxThingSpeak, "HE3ZUVZ1MKI1FOPB", 1, i);
  	//bTransmitCommand(&xThingSpeak, "AT\r\n", 4);

  	i += 20;
  	osDelay(20000);
  	//osDelay(20000);
  	printf("thingspeak\r\n");

  	//xTaskNotifyFromISR((TaskHandle_t)pxThingSpeak->xProcMessageTaskHandle, (uint32_t)pxThingSpeak->xRXBuffer.uHeadIndex, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
  	//portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

  	//xTaskNotifyGive((TaskHandle_t)procMessageTaskHandle);

  }
}




void vStartProcMessageTask(void *argument)
{
	uint32_t uHeadIndex = 0;
	ThingSpeakHandle_t *pxThingSpeak = argument;

  for(;;)
  {
  	if (xTaskNotifyWait(0, 0xffffffff, &uHeadIndex, pdMS_TO_TICKS(1000)) == pdTRUE)
  	{
  		bParseMessage(pxThingSpeak, (uint16_t)uHeadIndex);

    	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
      osDelay(100);
    	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
  	}
  	else
  	{
  		// Error Handle
  	}

  	printf("parse\r\n");

  	/*
  	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
    osDelay(100);
  	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
  	*/
  }
}


void vInitThingSpeak(ThingSpeakHandle_t *pxThingSpeak, UART_HandleTypeDef *huart, DMA_HandleTypeDef *pxUART_DMA_RX,
		const char *pcThingSpeakTaskName, uint32_t uThingSpeakTaskSize, osPriority_t xThingSpeakTaskPriority,
		const char *pcProcessMessageTaskName, uint32_t uProcessMessageTaskSize, osPriority_t xProcessMessageTaskPriority)
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

	pxThingSpeak->xThingSpeakTaskAttributes.name = pcThingSpeakTaskName;
	pxThingSpeak->xThingSpeakTaskAttributes.stack_size = uThingSpeakTaskSize;
	pxThingSpeak->xThingSpeakTaskAttributes.priority = xThingSpeakTaskPriority,
	pxThingSpeak->xThingSpeakTaskHandle = osThreadNew(vStartThingSpeakTask, pxThingSpeak, &pxThingSpeak->xThingSpeakTaskAttributes);

	pxThingSpeak->xProcMessageTaskAttributes.name = pcProcessMessageTaskName;
	pxThingSpeak->xProcMessageTaskAttributes.stack_size = uProcessMessageTaskSize;
	pxThingSpeak->xProcMessageTaskAttributes.priority = xProcessMessageTaskPriority,
	pxThingSpeak->xProcMessageTaskHandle = osThreadNew(vStartProcMessageTask, pxThingSpeak, &pxThingSpeak->xProcMessageTaskAttributes);


	// Receive DMA Buffer
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  HAL_UART_Receive_DMA(huart, pxThingSpeak->xRXBuffer.puDMABuffer, sizeof(pxThingSpeak->xRXBuffer.puDMABuffer));
}


void vRefreshThingSpeak(ThingSpeakHandle_t *pxThingSpeak)
{
	HAL_UART_DMAStop(pxThingSpeak->huart);

	memset(pxThingSpeak->xRXBuffer.puDMABuffer, 0, sizeof(pxThingSpeak->xRXBuffer.puDMABuffer));
	pxThingSpeak->xRXBuffer.uHeadIndex = 0;
	pxThingSpeak->xRXBuffer.uTailIndex = 0;
	pxThingSpeak->xRXBuffer.uRollOver = 0;
	memset(pxThingSpeak->xTXBuffer.puDMABuffer, 0, sizeof(pxThingSpeak->xTXBuffer.puDMABuffer));
	pxThingSpeak->xTXBuffer.uHeadIndex = 0;
	pxThingSpeak->xTXBuffer.uTailIndex = 0;
	pxThingSpeak->xTXBuffer.uRollOver = 0;

  __HAL_UART_ENABLE_IT(pxThingSpeak->huart, UART_IT_IDLE);
  HAL_UART_Receive_DMA(pxThingSpeak->huart, pxThingSpeak->xRXBuffer.puDMABuffer, sizeof(pxThingSpeak->xRXBuffer.puDMABuffer));
}


void USER_UART_IDLECallback(ThingSpeakHandle_t *pxThingSpeak)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// Tail catch up to head
	pxThingSpeak->xRXBuffer.uHeadIndex = sizeof(pxThingSpeak->xRXBuffer.puDMABuffer) - __HAL_DMA_GET_COUNTER(pxThingSpeak->pxUART_DMA_RX);

	// Overwrite the most recent viable head (mailbox)
	xTaskNotifyFromISR((TaskHandle_t)pxThingSpeak->xProcMessageTaskHandle, (uint32_t)pxThingSpeak->xRXBuffer.uHeadIndex, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


uint8_t bParseMessage(ThingSpeakHandle_t *pxThingSpeak, uint16_t uHeadIndex)
{
	uint16_t uTailIndex = pxThingSpeak->xRXBuffer.uTailIndex;
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

					vHandleCandidateCommand(pxThingSpeak, candidate, candidateLength);
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

						vHandleCandidateCommand(pxThingSpeak, candidate, candidateLength);
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

							vHandleCandidateCommandSplit(pxThingSpeak, candidateFirst, candidateFirstLength, candidateSecond, candidateSecondLength);

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

							vHandleCandidateCommand(pxThingSpeak, candidate, candidateLength);
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
			//HAL_UART_DMAStop(pxThingSpeak->huart);
			//vInitThingSpeak(pxThingSpeak, pxThingSpeak->huart, pxThingSpeak->pxUART_DMA_RX, pxThingSpeak->xProcMessageTaskHandle);
			vRefreshThingSpeak(pxThingSpeak);
		}
	}
	else
	{
		// Reset due to too overflow rx buffer due to too much data received before it could all process
		//HAL_UART_DMAStop(pxThingSpeak->huart);
		//vInitThingSpeak(pxThingSpeak, pxThingSpeak->huart, pxThingSpeak->pxUART_DMA_RX, pxThingSpeak->xProcMessageTaskHandle);
		vRefreshThingSpeak(pxThingSpeak);
	}

	printf("TailIndex: %u, HeadIndex: %u\r\n", pxThingSpeak->xRXBuffer.uTailIndex, pxThingSpeak->xRXBuffer.uHeadIndex);

	return 0;
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


void vHandleCandidateCommand(ThingSpeakHandle_t *pxThingSpeak, const char *candidate, size_t candidateLength)
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
		//printf("%lu\r\n", (uint32_t)pxThingSpeak->xThingSpeakTaskHandle);

		/* Task Notify */
		xTaskNotify((TaskHandle_t)pxThingSpeak->xThingSpeakTaskHandle, (uint32_t)OK, eSetValueWithOverwrite);
	}
	else
	{
		//HAL_UART_Transmit(&huart2, "INVLD: ", 7, 1000);
		HAL_UART_Transmit(&huart2, (char *)candidate, candidateLength, 1000);
		HAL_UART_Transmit(&huart2, "\r\n", 2, 1000);
		//printf("INVLD\r\n");
	}
}


void vHandleCandidateCommandSplit(ThingSpeakHandle_t *pxThingSpeak, const char *candidateFirst, size_t candidateFirstLength,
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
		/* TaskNotify */
		xTaskNotify((TaskHandle_t)pxThingSpeak->xThingSpeakTaskHandle, (uint32_t)OK, eSetValueWithOverwrite);
	}
	else
	{
		HAL_UART_Transmit(&huart2, (char *)candidateFirst, candidateFirstLength, 1000);
		HAL_UART_Transmit(&huart2, (char *)candidateSecond, candidateSecondLength, 1000);
		HAL_UART_Transmit(&huart2, "\r\n", 2, 1000);
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


uint8_t bReceiveThingSpeakCommand(ThingSpeakHandle_t *pxThingSpeak, ATCommand_t eATCommand, TickType_t xTicksToWait)
{
	// Another Task for "Event group" Or Mailbox notification that waits for

	// "Expects" said data, and will wait for a period of time for it
	// If it does not get the data in time or gets the wrong data (based on xTaskNotifyWait), then return 0.
	// If success then return 1.


	// Enum for message
	uint32_t uCommand;
	if (xTaskNotifyWait(0, 0xffffffff, &uCommand, xTicksToWait) == pdPASS)
	{
		if ((ATCommand_t)uCommand == eATCommand)
		{
			return 1;
		}
		else
		{
			/* Error Handle: Wrong command expected */
			return 0;
		}
	}
	else
	{
		/* Error Handle: Timeout */
		return 0;
	}

}


uint8_t bTransmitThingSpeakData(ThingSpeakHandle_t *pxThingSpeak, char *apiKey, uint8_t field, uint16_t value)
{
	//osDelay(3000);
	char local_buf[100] = {0};
	char local_buf2[30] = {0};

	bTransmitCommand(pxThingSpeak, "AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n", sizeof("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n"));
	bReceiveThingSpeakCommand(pxThingSpeak, OK, pdMS_TO_TICKS(3000));
	//osDelay(3000);

	//sprintf(local_buf, "GET /update?api_key=%sfield%u=%u\r\n", apiKey, field, value);
	sprintf(local_buf, "GET https://api.thingspeak.com/update?api_key=%s&field%u=%u\r\n", apiKey, field, value);
	int len = strlen(local_buf);

	sprintf(local_buf2, "AT+CIPSEND=%d\r\n", len);
	bTransmitCommand(pxThingSpeak, local_buf2, strlen(local_buf2));
	bReceiveThingSpeakCommand(pxThingSpeak, OK, pdMS_TO_TICKS(3000));
	//osDelay(3000);

	bTransmitCommand(pxThingSpeak, local_buf, strlen(local_buf));
	bReceiveThingSpeakCommand(pxThingSpeak, OK, pdMS_TO_TICKS(3000));
	//osDelay(3000);



	return 1;
}






