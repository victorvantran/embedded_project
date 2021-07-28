/*
 * ws2812b.h
 *
 *  Created on: Jul 27, 2021
 *      Author: Victor
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include "math.h"
#include "stm32l4xx_hal.h"
#include "io_mapping.h"


#define FCPU_HZ 80000000



#define WS2812B_LED_COUNT 8
#define WS2812B_USE_BRIGHTNESS 1
#define WS2812B_PRESCALE 1
#define WS2812B_TIMER_CHANNEL TIM_CHANNEL_1


/* IMPLEMENTATION */
#if WS2812B_LED_COUNT >= 128 - 1
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



struct LEDData
{
uint8_t green;
uint8_t red;
uint8_t blue;
uint8_t index;
};


class WS2812B
{
private:
	uint32_t _FREQUENCY;
	uint32_t _LED_COUNT;

	TIM_HandleTypeDef* _timer;

	uint16_t _pwmData[(24*WS2812B_LED_COUNT) + WS2812B_RESET_PERIOD_COUNT];


	LEDData _colorData[WS2812B_LED_COUNT];
	LEDData _brightnessData[WS2812B_LED_COUNT];

public:
	WS2812B(uint32_t frequency, uint32_t ledCount, TIM_HandleTypeDef* timer) : _FREQUENCY(frequency), _LED_COUNT(ledCount), _timer(timer) {}
	~WS2812B() {}

	/*
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


		// Reset signal
		for (; i < 24 + WS2812B_RESET_PERIOD_COUNT; i++)
		{
				this->_pwmData[i] = 0;
		}

		HAL_TIM_PWM_Start_DMA(this->_timer, WS2812B_TIMER_CHANNEL, (uint32_t*)this->_pwmData, 24 + WS2812B_RESET_PERIOD_COUNT);
	}
	*/



	void setColor(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
	{
		this->_colorData[index].index = index;
		this->_colorData[index].green = green;
		this->_colorData[index].red = red;
		this->_colorData[index].blue = blue;
	}


	void setBrightness(uint8_t brightness)
	{
	#if WS2812B_USE_BRIGHTNESS
		if (brightness > 45) brightness = 45;
		for (int i = 0; i < this->_LED_COUNT; i++)
		{
			this->_brightnessData[i].index = this->_colorData[i].index;
			float angle = 90 - brightness;
			angle = angle*M_PI / 180;
			this->_brightnessData[i].green = (this->_colorData[i].green)/(tan(angle));
			this->_brightnessData[i].red = (this->_colorData[i].red)/(tan(angle));
			this->_brightnessData[i].blue = (this->_colorData[i].blue)/(tan(angle));
		}
	#else
		for (int i = 0; i < this->_LED_COUNT; i++)
		{
			this->_brightnessData[i].index = this->_colorData[i].index;
			this->_brightnessData[i].green = this->_colorData[i].green;
			this->_brightnessData[i].red = this->_colorData[i].red;
			this->_brightnessData[i].blue = this->_colorData[i].blue;
		}
	#endif
	}




	void resetColor(uint8_t index)
	{
		this->_colorData[index].green = 0;
		this->_colorData[index].red = 0;
		this->_colorData[index].blue = 0;
	}

	void resetColors()
	{
		for (int i = 0; i < this->_LED_COUNT; i++)
		{
			this->resetColor(i);
		}
	}

	void update()
	{
		uint32_t colorData = 0;
		int32_t bit_index = 0;

		for (int led_index = 0; led_index < this->_LED_COUNT; led_index++)
		{
			colorData = (((uint32_t)this->_brightnessData[led_index].green << 16) | ((uint32_t)this->_brightnessData[led_index].red << 8) | ((uint32_t)this->_brightnessData[led_index].blue << 0));
			//colorData = (((uint32_t)this->_colorData[led_index].green << 16) | ((uint32_t)this->_colorData[led_index].red << 8) | ((uint32_t)this->_colorData[led_index].blue << 0));

			for (int j = 23; j >= 0; j--)
			{
				if (colorData & (1 << j))
				{
					this->_pwmData[bit_index] = WS2812B_T1H_TICKS;
				}
				else
				{
					this->_pwmData[bit_index] = WS2812B_T0H_TICKS;
				}
				bit_index++;
			}
		}


		for (int32_t resetCount = 0; resetCount < WS2812B_RESET_PERIOD_COUNT; resetCount++)
		{
				this->_pwmData[bit_index++] = 0;
		}


		HAL_TIM_PWM_Start_DMA(this->_timer, WS2812B_TIMER_CHANNEL, (uint32_t*)this->_pwmData, bit_index);

		HAL_Delay(100);
		// BLOCK for a period of time

		// RESET
		 resetColors();
	}



};




#endif /* INC_WS2812B_H_ */
