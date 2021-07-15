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

void MainMenuState::vEnter(UI* pxUI) const
{

}


void MainMenuState::vExit(UI* pxUI) const
{

}


void MainMenuState::vEventUp(UI* pxUI) const
{

}


void MainMenuState::vEventDown(UI* pxUI) const
{

}


void MainMenuState::vEventLeft(UI* pxUI) const
{
	pxUI->vTransitionState(MainMenuState::getInstance());
}


void MainMenuState::vEventRight(UI* pxUI) const
{


}


const UIState& MainMenuState::getInstance(void)
{
	static const MainMenuState xSingleton;
	return xSingleton;
}


// MusicState
MusicState::MusicState() : UIState(EnumState::MUSIC) {}
MusicState::~MusicState() {}

void MusicState::vEnter(UI* pxUI) const
{

}


void MusicState::vExit(UI* pxUI) const
{

}


void MusicState::vEventUp(UI* pxUI) const
{

}


void MusicState::vEventDown(UI* pxUI) const
{

}


void MusicState::vEventLeft(UI* pxUI) const
{

}


void MusicState::vEventRight(UI* pxUI) const
{


}


// ProfileState


// MainMenuState
ProfileState::ProfileState() : UIState(EnumState::PROFILE) {}
ProfileState::~ProfileState() {}

void ProfileState::vEnter(UI* pxUI) const
{

}


void ProfileState::vExit(UI* pxUI) const
{

}


void ProfileState::vEventUp(UI* pxUI) const
{

}


void ProfileState::vEventDown(UI* pxUI) const
{

}


void ProfileState::vEventLeft(UI* pxUI) const
{

}


void ProfileState::vEventRight(UI* pxUI) const
{


}



// SettingsState
SettingsState::SettingsState() : UIState(EnumState::SETTINGS) {}
SettingsState::~SettingsState() {}

void SettingsState::vEnter(UI* pxUI) const
{

}


void SettingsState::vExit(UI* pxUI) const
{

}


void SettingsState::vEventUp(UI* pxUI) const
{

}


void SettingsState::vEventDown(UI* pxUI) const
{

}


void SettingsState::vEventLeft(UI* pxUI) const
{

}


void SettingsState::vEventRight(UI* pxUI) const
{


}



// UI
/*
 * Singleton pattern
UI::UI() : _xMainMenu(), _xMusic(), _xProfile(), _xSettings(), _pxCurrentState(&_xMainMenu),
		_pxUART(nullptr) {}


UI::UI(UART_HandleTypeDef *pxUART) : _xMainMenu(), _xMusic(), _xProfile(), _xSettings(), _pxCurrentState(&_xMainMenu),
		_pxUART(pxUART)
{
	this->_pxCurrentState->vEnter(this);
}
*/

UI::UI() : _pxCurrentState(&MainMenuState::getInstance()), _pxUART(nullptr) {}


UI::UI(UART_HandleTypeDef *pxUART) : _pxCurrentState(&MainMenuState::getInstance()), _pxUART(pxUART)
{
	this->_pxCurrentState->vEnter(this);
}


UI::~UI()
{
	this->_pxCurrentState->vExit(this);
}


void UI::test(void) const
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("test\r\n"), sizeof("test\r\n"), 100);
}


const UIState* UI::pxGetCurrentState(void) const
{
	return this->_pxCurrentState;
}


void UI::vTransitionState(const UIState& xNextState)
{
	this->_pxCurrentState->vExit(this);
	this->_pxCurrentState = &xNextState;
	this->_pxCurrentState->vEnter(this);
}


void UI::vPressUpButton(void)
{
	this->_pxCurrentState->vEventUp(this);
}


void UI::vPressDownButton(void)
{
	this->_pxCurrentState->vEventDown(this);
}


void UI::vPressLeftButton(void)
{
	this->_pxCurrentState->vEventLeft(this);
}


void UI::vPressRightButton(void)
{
	this->_pxCurrentState->vEventRight(this);
}



