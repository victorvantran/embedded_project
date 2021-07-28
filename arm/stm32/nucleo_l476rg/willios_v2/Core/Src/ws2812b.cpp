/*
 * ws2812b.cpp
 *
 *  Created on: Jul 27, 2021
 *      Author: Victor
 */


#include "ws2812b.h"


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		HAL_TIM_PWM_Stop_DMA(htim, WS2812B_TIMER_CHANNEL);
	}
}
