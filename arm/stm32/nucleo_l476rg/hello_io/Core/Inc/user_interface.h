/*
 * user_interface.h
 *
 *  Created on: Jul 11, 2021
 *      Author: victor
 */

#ifndef INC_USER_INTERFACE_H_
#define INC_USER_INTERFACE_H_



// Elecia White Figure 4-3

// LED
// stm32l4xx_hal_conf // Processor Header
#include "stm32l4xx_hal.h" // IO Pin Handler
#include "io_mapping.h" // IO Mapping




// State varibles (dependencies may be introduced!)
// State machine






void UI_set_LED0(void)
{
	HAL_GPIO_WritePin(LED0_PORT, LED0_PIN, GPIO_PIN_SET);
}


void UI_clear_LED0(void)
{
	HAL_GPIO_WritePin(LED0_PORT, LED0_PIN, GPIO_PIN_RESET);
}


void UI_toggle_LED0(void)
{
	HAL_GPIO_TogglePin(LED0_PORT, LED0_PIN);
}


GPIO_PinState UI_get_LED0_BUTTON(void)
{
	return HAL_GPIO_ReadPin(BUTTON_LED0_PORT, BUTTON_LED0_PIN);
}


_Bool UI_is_LED0_BUTTON_pressed(void)
{
	// Idle HIGH/Active LOW via internal pull-up
	return !HAL_GPIO_ReadPin(BUTTON_LED0_PORT, BUTTON_LED0_PIN);
}




void UI_poll_LED0(void)
{
	if (UI_is_LED0_BUTTON_pressed())
	{
  	UI_set_LED0();
	}
	else
	{
  	UI_clear_LED0();
	}
}





#endif /* INC_USER_INTERFACE_H_ */
