/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "fatfs_sd.h"
#include "piece.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef __GNUC__
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;
char buffer[100];


PieceHandle_t xPiece;


// sending to UART
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}


void transmit_uart(char *string) {
	uint8_t len = strlen(string);
	HAL_UART_Transmit(&huart2, (uint8_t*) string, len, 200);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
typedef struct
{
	union
	{
		uint8_t a[2];
		uint16_t b;
	}
	//uint16_t a;
} TestHandle_t;

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */

	 /* Waiting for the Micro SD module to initialize */

		/*
		HAL_Delay(500);

		fres = f_mount(&fs, "", 0);
		if (fres == FR_OK) {
			transmit_uart("Micro SD card is mounted successfully!\n");
		} else if (fres != FR_OK) {
			transmit_uart("Micro SD card's mount error!\n");
		}

		// FA_OPEN_APPEND opens file if it exists and if not then creates it,
		// the pointer is set at the end of the file for appending
		fres = f_open(&fil, "log-file.txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);
		if (fres == FR_OK) {
			transmit_uart("File opened for reading and checking the free space.\n");
		} else if (fres != FR_OK) {
			transmit_uart("File was not opened for reading and checking the free space!\n");
		}

		fres = f_getfree("", &fre_clust, &pfs);
		totalSpace = (uint32_t) ((pfs->n_fatent - 2) * pfs->csize * 0.5);
		freeSpace = (uint32_t) (fre_clust * pfs->csize * 0.5);
		char mSz[12];
		sprintf(mSz, "%lu", freeSpace);
		if (fres == FR_OK) {
			transmit_uart("The free space is: ");
			transmit_uart(mSz);
			transmit_uart("\n");
		} else if (fres != FR_OK) {
			transmit_uart("The free space could not be determined!\n");
		}

		for (uint8_t i = 0; i < 3; i++) {
			f_puts("This text is written in the file.\n", &fil);
		}

		fres = f_close(&fil);
		if (fres == FR_OK) {
			transmit_uart("The file is closed.\n");
		} else if (fres != FR_OK) {
			transmit_uart("The file was not closed.\n");
		}

		// Open file to read
		fres = f_open(&fil, "log-file.txt", FA_READ);
		if (fres == FR_OK) {
			transmit_uart("File opened for reading.\n");
		} else if (fres != FR_OK) {
			transmit_uart("File was not opened for reading!\n");
		}

		while (f_gets(buffer, sizeof(buffer), &fil)) {
			char mRd[100];
			sprintf(mRd, "%s", buffer);
			transmit_uart(mRd);

		}

		// Close file
		fres = f_close(&fil);
		if (fres == FR_OK) {
			transmit_uart("The file is closed.\n");
		} else if (fres != FR_OK) {
			transmit_uart("The file was not closed.\n");
		}

		f_mount(NULL, "", 1);
		if (fres == FR_OK) {
			transmit_uart("The Micro SD card is unmounted!\n");
		} else if (fres != FR_OK) {
			transmit_uart("The Micro SD was not unmounted!");
		}

		*/

  /* Infinite loop */
  for(;;)
  {
  	osDelay(2000);


  	// Mount
 		fres = f_mount(&fs, "", 0);
 		if (fres == FR_OK) {
 			transmit_uart("Micro SD card is mounted successfully!\n");
 		} else if (fres != FR_OK) {
 			transmit_uart("Micro SD card's mount error!\n");
 		}


 		/* Open file to read */
 		/*
 		fres = f_open(&fil, "log-file.txt", FA_READ);
 		if (fres == FR_OK) {
 			transmit_uart("File opened for reading.\n");
 		} else if (fres != FR_OK) {
 			transmit_uart("File was not opened for reading!\n");
 		}

 		while (f_gets(buffer, sizeof(buffer), &fil)) {
 			char mRd[100];
 			sprintf(mRd, "%s", buffer);
 			transmit_uart(mRd);
 		}
 		*/

 		fres = f_open(&fil, "piece_test.txt", FA_READ);
 		if (fres == FR_OK) {
 			transmit_uart("File opened for reading.\n");
 		} else if (fres != FR_OK) {
 			transmit_uart("File was not opened for reading!\n");
 		}


 		/*
 		char c;
 		while (f_gets(&c, sizeof(c), &fil)) {
 	 		printf("Character: %u\r\n", c);
 		}
 		*/




 		/*
 		uint8_t usNumBytesRead;
 		uint32_t ulCompositionSize;
 		uint16_t uBPM;
 		f_read(&fil, &uBPM, sizeof(uBPM), &usNumBytesRead);
 	 	printf("BPM: %u\r\n", uBPM);
 		f_read(&fil, &ulCompositionSize, sizeof(ulCompositionSize), &usNumBytesRead);
 	 	printf("Composition Size: %lu\r\n", ulCompositionSize);
		*/


 		uint8_t usNumBytesRead;
 		f_read(&fil, xPiece.xPieceInformation.pusName, sizeof(xPiece.xPieceInformation.pusName), &usNumBytesRead);
 	 	printf("Name: %s\r\n", xPiece.xPieceInformation.pusName);



 	 	/*
 		TestHandle_t c;
 		uint8_t usNumBytesRead;

 		uint8_t a[2];

 		uint16_t b;

 		// NEed to rearrange the file for Little endian
 		f_read(&fil, &c, sizeof(c), &usNumBytesRead);
 	 	printf("Character: %u\r\n", c.a[0]);
 	 	printf("Character: %u\r\n", c.a[1]);
 	 	printf("Character: %u\r\n", c.b);
		*/

 	 	//printf("Character: %u\r\n", a[0]);
 	 	//printf("Character: %u\r\n", a[1]);



 		//f_read(&fil, &c, sizeof(c), &usNumBytesRead);
 	 	//printf("Character: %u\r\n", c);
 			/*
 		uint16_t uBPM;
 		uint32_t ulNumNotes;



 		//f_gets(&uBPM, sizeof(uBPM), &fil);
 		f_gets(&uBPM, 2, &fil);
 		//f_gets(&ulNumNotes, sizeof(ulNumNotes), &fil);
 		f_gets(&ulNumNotes, 4, &fil);

 		printf("BMP: %u\r\n", uBPM);
 		printf("Num Notes: %lu\r\n", ulNumNotes);
 		*/

 		/*
 		while (f_gets(buffer, sizeof(buffer), &fil)) {
 			char mRd[100];
 			sprintf(mRd, "%s\r\n", buffer);
 			transmit_uart(mRd);
 		}
 		*/

 		/* Close file */
 		fres = f_close(&fil);
 		if (fres == FR_OK) {
 			transmit_uart("The file is closed.\n");
 		} else if (fres != FR_OK) {
 			transmit_uart("The file was not closed.\n");
 		}

 		f_mount(NULL, "", 1);
 		if (fres == FR_OK) {
 			transmit_uart("The Micro SD card is unmounted!\n");
 		} else if (fres != FR_OK) {
 			transmit_uart("The Micro SD was not unmounted!");
 		}





    osDelay(2000);
  }
  /* USER CODE END 5 */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
