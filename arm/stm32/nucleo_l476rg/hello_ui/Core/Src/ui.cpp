/*
 * ui.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: victor
 */

#include "ui.h"



// UI State
UIState::UIState(EnumState eState) : _eState(eState) {}
UIState::~UIState() {}


MainMenuState::MainMenuState() : UIState(EnumState::MAIN_MENU) {}
MainMenuState::~MainMenuState() {}

MusicState::MusicState() : UIState(EnumState::MUSIC) {}
MusicState::~MusicState() {}

SettingsState::SettingsState() : UIState(EnumState::SETTINGS) {}
SettingsState::~SettingsState() {}



// UI
UI::UI() : _xMainMenu(), _xMusic(), _xSettings(), _pxUART(nullptr) {}


UI::UI(UART_HandleTypeDef *pxUART) : _xMainMenu(), _xMusic(), _xSettings(), _pxUART(pxUART) {}


UI::~UI() {}


void UI::test(void)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("test\r\n"), sizeof("test\r\n"), 100);
}
