/*
 * ui.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: victor
 */

#include "ui.h"



// UIState
UIState::UIState(EnumState eState) : _eState(eState) {}
UIState::~UIState() {}




// MainMenuState
MainMenuState::MainMenuState() : UIState(EnumState::MAIN_MENU) {}
MainMenuState::~MainMenuState() {}

void MainMenuState::vEnter(void)
{

}


void MainMenuState::vExit(void)
{

}


void MainMenuState::vEventUp(void)
{

}


void MainMenuState::vEventDown(void)
{

}


void MainMenuState::vEventLeft(void)
{

}


void MainMenuState::vEventRight(void)
{


}


// MusicState
MusicState::MusicState() : UIState(EnumState::MUSIC) {}
MusicState::~MusicState() {}

void MusicState::vEnter(void)
{

}


void MusicState::vExit(void)
{

}


void MusicState::vEventUp(void)
{

}


void MusicState::vEventDown(void)
{

}


void MusicState::vEventLeft(void)
{

}


void MusicState::vEventRight(void)
{


}


// ProfileState


// MainMenuState
ProfileState::ProfileState() : UIState(EnumState::PROFILE) {}
ProfileState::~ProfileState() {}

void ProfileState::vEnter(void)
{

}


void ProfileState::vExit(void)
{

}


void ProfileState::vEventUp(void)
{

}


void ProfileState::vEventDown(void)
{

}


void ProfileState::vEventLeft(void)
{

}


void ProfileState::vEventRight(void)
{


}



// SettingsState
SettingsState::SettingsState() : UIState(EnumState::SETTINGS) {}
SettingsState::~SettingsState() {}

void SettingsState::vEnter(void)
{

}


void SettingsState::vExit(void)
{

}


void SettingsState::vEventUp(void)
{

}


void SettingsState::vEventDown(void)
{

}


void SettingsState::vEventLeft(void)
{

}


void SettingsState::vEventRight(void)
{


}



// UI
UI::UI() : _xMainMenu(), _xMusic(), _xProfile(), _xSettings(), _pxCurrState(&_xMainMenu),
		_pxUART(nullptr) {}


UI::UI(UART_HandleTypeDef *pxUART) : _xMainMenu(), _xMusic(), _xProfile(), _xSettings(), _pxCurrState(&_xMainMenu),
		_pxUART(pxUART)
{
	this->_pxCurrState->vEnter();
}


UI::~UI()
{
	this->_pxCurrState->vExit();
}


void UI::test(void)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("test\r\n"), sizeof("test\r\n"), 100);
}






