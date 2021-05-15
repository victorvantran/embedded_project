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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#ifdef __GNUC__
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* Definitions for xServo */
osThreadId_t xServoHandle;
const osThreadAttr_t xServo_attributes = {
  .name = "xServo",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for pidCalculate */
osThreadId_t pidCalculateHandle;
const osThreadAttr_t pidCalculate_attributes = {
  .name = "pidCalculate",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for xEventGroupPID */
osEventFlagsId_t xEventGroupPIDHandle;
const osEventFlagsAttr_t xEventGroupPID_attributes = {
  .name = "xEventGroupPID"
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
void StartServoTask(void *argument);
void StartPIDCalculateTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))


#define mainULTRASONIC_COMMANDED_ISR_BIT (1UL << 0UL)
#define mainULTRASONIC_CONTROLLED_ISR_BIT (1UL << 1UL)


typedef enum
{
	CAPTURE_RISING_EDGE,
	CAPTURE_FALLING_EDGE,
} UltrasonicState_t;


typedef struct
{
	struct
	{
		GPIO_TypeDef *pxPort;
		uint16_t uPin;
	};

	uint32_t ulICValRise;
	uint32_t ulICValFall;
	uint32_t ulPulseWidthMS;
	float fDistanceCM;
	UltrasonicState_t eUltrasonicState;
} UltrasonicSensor_t;


// Input
volatile UltrasonicSensor_t xUltrasonicSensorCommanded =
{
		.pxPort = GPIOA,
		.uPin = GPIO_PIN_10,
		.ulICValRise = 0UL,
		.ulICValFall = 0UL,
		.ulPulseWidthMS = 0UL,
		.fDistanceCM = 0UL,
		.eUltrasonicState = CAPTURE_RISING_EDGE
};


// Controlled variable
volatile UltrasonicSensor_t xUltrasonicSensorControlled =
{
		.pxPort = GPIOA,
		.uPin = GPIO_PIN_11,
		.ulICValRise = 0UL,
		.ulICValFall = 0UL,
		.ulPulseWidthMS = 0UL,
		.fDistanceCM = 0UL,
		.eUltrasonicState = CAPTURE_RISING_EDGE
};




static void updateUltrasonicSensor(volatile UltrasonicSensor_t *ultrasonicSensor, uint32_t captureVal)
{
	if (ultrasonicSensor->eUltrasonicState == CAPTURE_RISING_EDGE)
	{
		ultrasonicSensor->ulICValRise = captureVal;
		ultrasonicSensor->eUltrasonicState = CAPTURE_FALLING_EDGE;
	}
	else if (ultrasonicSensor->eUltrasonicState == CAPTURE_FALLING_EDGE)
	{
		ultrasonicSensor->ulICValFall = captureVal;
		if (ultrasonicSensor->ulICValFall >= ultrasonicSensor->ulICValRise)
		{
			ultrasonicSensor->ulPulseWidthMS = ultrasonicSensor->ulICValFall - ultrasonicSensor->ulICValRise;
		}
		else
		{
			ultrasonicSensor->ulPulseWidthMS = (0xFFFF - ultrasonicSensor->ulICValRise) + ultrasonicSensor->ulICValFall;
		}
		//ultrasonicSensor->fDistanceCM = (float)ultrasonicSensor->ulPulseWidthMS / 58.30875f;
		ultrasonicSensor->eUltrasonicState = CAPTURE_RISING_EDGE;
	}
}



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint32_t captureVal;
	switch((intptr_t)htim->Instance)
	{
	case (intptr_t)TIM1:
		switch((uint32_t)htim->Channel)
		{
		case (uint32_t)HAL_TIM_ACTIVE_CHANNEL_1:
			captureVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			updateUltrasonicSensor(&xUltrasonicSensorCommanded, captureVal);

			// Pulse width entirely captured, so set event group/flag
			if (xUltrasonicSensorCommanded.eUltrasonicState == CAPTURE_RISING_EDGE)
			{
				osEventFlagsSet(xEventGroupPIDHandle, mainULTRASONIC_COMMANDED_ISR_BIT);
			}
			break;
		case (uint32_t)HAL_TIM_ACTIVE_CHANNEL_2:
			captureVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			updateUltrasonicSensor(&xUltrasonicSensorControlled, captureVal);

			// Pulse width entirely captured, so set event group/flag
			if (xUltrasonicSensorControlled.eUltrasonicState == CAPTURE_RISING_EDGE)
			{
				osEventFlagsSet(xEventGroupPIDHandle, mainULTRASONIC_CONTROLLED_ISR_BIT);
			}
			break;
		}
		break;
	}
}



float rollingAverage(float average, float newSample, int8_t n)
{
	average -= average/n;
	average += newSample/n;

	return average;
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  // Start input capture in interrupt mode
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);

  // Start PWM for servo
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

	htim3.Instance->CCR1 = 10;
	htim3.Instance->CCR2 = 10;

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
  /* creation of xServo */
  //xServoHandle = osThreadNew(StartServoTask, NULL, &xServo_attributes);

  /* creation of pidCalculate */
  pidCalculateHandle = osThreadNew(StartPIDCalculateTask, NULL, &pidCalculate_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  	  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of xEventGroupPID */
  xEventGroupPIDHandle = osEventFlagsNew(&xEventGroupPID_attributes);

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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 80 - 1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 80 - 1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 3000 - 1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 80 - 1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 60000 - 1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA10 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
  	PUTCHAR_PROTOTYPE
  	{
  		HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10);
  		return ch;
  	}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartServoTask */
  	/**
  	* @brief Function implementing the servo thread.
  	* @param argument: Not used
  	* @retval None
  	*/
/* USER CODE END Header_StartServoTask */
void StartServoTask(void *argument)
{
  /* USER CODE BEGIN 5 */
	uint8_t mode = 0;

	/* Infinite loop */
	for(;;)
	{

		/*
		if (mode == 0)
		{
			htim2.Instance->CCR1 = 2460;
		}
		else if (mode == 1)
		{
			htim2.Instance->CCR1 = 1815;
		}
		else if (mode == 2)
		{
			htim2.Instance->CCR1 = 1170;
		}
		else if (mode == 3)
		{
			htim2.Instance->CCR1 = 1815;
		}

		mode = (mode + 1) % 4;
		*/

		/*
		if (mode == 0)
		{
			htim2.Instance->CCR1 = 2015;
			osDelay(400);

		}
		else if (mode == 1)
		{
			htim2.Instance->CCR1 = 1615;
			osDelay(400);

		}
		else if (mode == 2)
		{
			htim2.Instance->CCR1 = 2015;
			osDelay(400);

		}

				mode = (mode + 1) % 3;

		 */


		/*
		if (mode == 0)
		{
			htim2.Instance->CCR1 = 2015;
			osDelay(10);
		}
		else if (mode == 1)
		{
			htim2.Instance->CCR1 = 1815;
			osDelay(10);
		}
		else if (mode == 2)
		{
			htim2.Instance->CCR1 = 1615;
			osDelay(10);
		}
		else if (mode == 3)
		{
			htim2.Instance->CCR1 = 1815;
			osDelay(10);
		}

		mode = (mode + 1) % 4;


		osDelay(400);
		*/


		/*
		if (mode == 0)
		{
			htim2.Instance->CCR1 = 1815 + 7*90;
		}
		else if (mode == 1)
		{
			htim2.Instance->CCR1 = 1815;
		}
		else if (mode == 2)
		{
			htim2.Instance->CCR1 = 1815 - 7*90;
		}
		else if (mode == 3)
		{
			htim2.Instance->CCR1 = 1815;
		}

		mode = (mode + 1) % 4;


		osDelay(1000);
		*/


		osDelay(1000);
		//printf("%lu\r\n", (uint32_t)htim2.Instance->CCR1);

		//osDelay(400);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartPIDCalculateTask */
  	/**
  	* @brief Function implementing the pidCalculate thread.
  	* @param argument: Not used
  	* @retval None
  	*/
/* USER CODE END Header_StartPIDCalculateTask */
void StartPIDCalculateTask(void *argument)
{
  /* USER CODE BEGIN StartPIDCalculateTask */
  /* Infinite loop */
	const uint32_t ulFlags = mainULTRASONIC_COMMANDED_ISR_BIT | mainULTRASONIC_CONTROLLED_ISR_BIT;

	//

	/*
	const float lKP = 0.5f;
	const float lKD = 1.01f;
	 */

	/*
	const float lKP = 0.25f;
	const float lKD = 0.50f;
	 */

	/*
	const float lKP = 0.15f;
	const float lKD = 5.00f;
	if (lError - lPrevError < 100) pidD = 0;
	*/

	/*
	const float lKP = 0.15f;
	const float lKD = 5.00f;
	if (lError - lPrevError < 40) pidD = 0;
	*/


	/*
	const float lKP = 0.25f;
	const float lKD = 2.50f;
	if (lError - lPrevError < 100) pidD = 0;
*/

	/*
	const float lKP = 0.25f;
	const float lKD = 3.50f;
	 */


	/*
	const float lKP = 0.15f;
	const float lKD = 0.50f;
	const int32_t lClampD = 50;
	*/

	/*
	const float lKP = 0.35f;//0.2f;
	const float lKD = 1.25f;//1.0f;
	const int32_t lClampD = 40;
	*/

	/*
	const float lKP = 0.10f;//0.2f;
	const float lKD = 0.75f;//1.0f;
	const int32_t lClampD = 20;
	*/

	/*
	const float lKP = 0.255f;//0.2f;
	const float lKD = 2.0f;//1.0f;
	*/

	const float lKP = 0.20f;//0.2f;
	const float lKD = 1.10f;//1.0f;
	const float lKI = 0.0001f;
	const int32_t lClampD = 25;

	int32_t pidP = 0;
	int32_t pidI = 0;
	int32_t pidD = 0;
	int32_t pidTotal = 0;


	int32_t lPrevError = 0;


	int32_t lAPrevError = 0;
	int32_t lBPrevError = 0;
	int32_t lCPrevError = 0;
	int32_t lDPrevError = 0;
	int32_t lEPrevError = 0;


	float aError = 0;

  for(;;)
  {
  	if (osEventFlagsWait(xEventGroupPIDHandle, ulFlags, osFlagsWaitAll, 400) == ulFlags)
  	{
  		// Clamp sensors
  		uint32_t commandedDistance = MAX(MIN(xUltrasonicSensorCommanded.ulPulseWidthMS, 3350), 350);
  		uint32_t controlledDistance = MAX(MIN(xUltrasonicSensorControlled.ulPulseWidthMS, 3350), 350);


  		int32_t lError = (controlledDistance - commandedDistance);


  		// Filter
  		lAPrevError = lBPrevError;
  		lBPrevError = lCPrevError;
  		lCPrevError = lDPrevError;
  		lDPrevError = lEPrevError;
  		lEPrevError = lError;
  		int32_t avgDeltaError = (int32_t)(0.25f*(lEPrevError - lDPrevError) + 0.25f*(lDPrevError - lCPrevError) + 0.25f*(lCPrevError - lBPrevError) + 0.25f*(lBPrevError - lAPrevError));


  		pidP = (int32_t)(lKP * lError);

  		/*
  		 * saturation and windup
  		pidI = (int32_t)(pidI + lKI * lError);


  		if (pidI > 0)
  		{
    		pidI = MAX(pidI, 150);

  		}
  		else
  		{
    		pidI = MIN(pidI, -150);
  		}
  		*/

  		pidD = (int32_t)(lKD * (avgDeltaError)); // (time is constant)

  		if ((avgDeltaError < lClampD) && (avgDeltaError > -lClampD)) pidD = 0;


  		pidTotal = pidP + pidI + pidD;

			htim2.Instance->CCR1 = MIN(2460, MAX((1815 + pidTotal), 1170));


			lPrevError = lError;

  		//printf("Controlled: %lu\r\n", xUltrasonicSensorControlled.ulPulseWidthMS);
  		//printf("ERROR: %d\r\n", lError);
  	}
  	//osDelay(200);
  }
  /* USER CODE END StartPIDCalculateTask */
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
