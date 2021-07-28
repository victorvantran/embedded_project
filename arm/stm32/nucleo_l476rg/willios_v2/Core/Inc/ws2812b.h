/*
 * ws2812b.h
 *
 *  Created on: Jul 27, 2021
 *      Author: Victor
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include "stm32l4xx_hal.h"
#include "io_mapping.h"


#define FCPU_HZ 80000000



#define WS2812B_LED_COUNT 1
#define WS2812B_USE_BRIGHTNESS 1
#define WS2812B_PRESCALE 1
#define WS2812B_TIMER_CHANNEL TIM_CHANNEL_1


/* IMPLEMENTATION */
#if LED_COUNT >= (126 - 1)
#error "LED COUNT should fit within an 8 bit signed number"
#endif

/*
 * Note: Change the Auto-reload register to the proper value that gives a period of 1.25 microseconds -> (ex: 100 ticks at 80MHZ gives 1.25 microseconds)
 */
#define WS2812B_TRANSFER_PERIOD_MICRO_SECONDS 1.25
#define WS2812B_TRANSFER_FREQUENCY_HZ (uint32_t)((1/WS2812B_TRANSFER_PERIOD_MICRO_SECONDS)*1000000)
#define WS2812B_ARR (uint32_t)(FCPU_HZ)/(WS2812B_TRANSFER_FREQUENCY_HZ)
#define WS2812B_T0H (float)(0.4/1.25)
#define WS2812B_T0L	(float)(0.85/1.25)
#define WS2812B_T1H (float)(0.8/1.25)
#define WS2812B_T1L	(float)(0.45/1.25)
#define WS2812B_RESET_PERIOD_MICRO_SECONDS 50
#define WS2812B_RESET_PERIOD_COUNT	(uint32_t)((float)(WS2812B_RESET_PERIOD_MICRO_SECONDS/WS2812B_TRANSFER_PERIOD_MICRO_SECONDS) + 1)
#define WS2812B_T0H_TICKS (uint16_t)((WS2812B_T0H*WS2812B_ARR)/WS2812B_PRESCALE)
#define WS2812B_T0L_TICKS (uint16_t)((WS2812B_T0L*WS2812B_ARR)/WS2812B_PRESCALE)
#define WS2812B_T1H_TICKS (uint16_t)((WS2812B_T1H*WS2812B_ARR)/WS2812B_PRESCALE)
#define WS2812B_T1L_TICKS (uint16_t)((WS2812B_T1L*WS2812B_ARR)/WS2812B_PRESCALE)


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);


class WS2812B
{
private:
	uint32_t _FREQUENCY;
	uint32_t _LED_COUNT;

	TIM_HandleTypeDef* _timer;

	uint16_t _pwmData[24 + WS2812B_RESET_PERIOD_COUNT];
public:
	WS2812B(uint32_t frequency, uint32_t ledCount, TIM_HandleTypeDef* timer) : _FREQUENCY(frequency), _LED_COUNT(ledCount), _timer(timer) {}
	~WS2812B() {}

	void send(uint32_t red, uint32_t green, uint32_t blue)
	{
		uint32_t data = (blue << 16) | (red << 8) | (green << 0); // Little endian send

		int32_t i;
		for (i = 0; i < 24; i++)
		{
			if (data & (1 << i))
			{
				this->_pwmData[i] = WS2812B_T1H_TICKS;
			}
			else
			{
				this->_pwmData[i] = WS2812B_T0H_TICKS;
			}
		}

		// Reset code
		for (; i < 24 + WS2812B_RESET_PERIOD_COUNT; i++)
		{
				this->_pwmData[i] = 0;
		}



		HAL_TIM_PWM_Start_DMA(this->_timer, WS2812B_TIMER_CHANNEL, (uint32_t*)this->_pwmData, 24 + WS2812B_RESET_PERIOD_COUNT);
	}
};




#endif /* INC_WS2812B_H_ */
