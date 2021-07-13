/*
 * ui.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: victor
 */

#include "ui.h"

UI::UI() {}


UI::~UI() {}


void UI::test(void)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

  //HAL_UART_Transmit(&huart2, (uint8_t *)("test\r\n"), sizeof("test\r\n"), 100);
}
