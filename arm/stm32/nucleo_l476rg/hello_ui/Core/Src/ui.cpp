/*
 * ui.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: victor
 */

#include "ui.h"



// UIState
UIState::UIState() {}
UIState::~UIState() {}




// MainMenuState
MainMenuState::MainMenuState() {}
MainMenuState::~MainMenuState() {}

void MainMenuState::vEnter(UI* pxUI) const
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("enter: main menu\r\n"), sizeof("enter: main menu\r\n"), 100);
}


void MainMenuState::vExit(UI* pxUI) const
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("exit: main menu\r\n"), sizeof("exit: main menu\r\n"), 100);
}


void MainMenuState::vEventUp(UI* pxUI) const
{
	pxUI->xMainMenu.vMoveUpStateOption();
}


void MainMenuState::vEventDown(UI* pxUI) const
{
	pxUI->xMainMenu.vMoveDownStateOption();
}


void MainMenuState::vEventLeft(UI* pxUI) const
{
	// NOP
	//pxUI->vTransitionState(MainMenuState::getInstance());
}


void MainMenuState::vEventRight(UI* pxUI) const
{
	switch (pxUI->xMainMenu.eStateOption)
	{
	case StateOption::MUSIC:
		pxUI->vTransitionState(MusicState::getInstance());
	break;
	case StateOption::PROFILE:
		pxUI->vTransitionState(ProfileState::getInstance());
	break;
	case StateOption::SETTINGS:
		pxUI->vTransitionState(SettingsState::getInstance());
	break;
	default:
	break;
	}
}


const UIState& MainMenuState::getInstance(void)
{
	static const MainMenuState xSingleton;
	return xSingleton;
}


// MusicState
MusicState::MusicState() {}
MusicState::~MusicState() {}

void MusicState::vEnter(UI* pxUI) const
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("enter: music\r\n"), sizeof("enter: music\r\n"), 100);
}


void MusicState::vExit(UI* pxUI) const
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("exit: music\r\n"), sizeof("exit: music\r\n"), 100);
}


void MusicState::vEventUp(UI* pxUI) const
{

}


void MusicState::vEventDown(UI* pxUI) const
{

}


void MusicState::vEventLeft(UI* pxUI) const
{
	pxUI->vTransitionState(MainMenuState::getInstance());
}


void MusicState::vEventRight(UI* pxUI) const
{


}

const UIState& MusicState::getInstance(void)
{
	static const MusicState xSingleton;
	return xSingleton;
}


// ProfileState


// MainMenuState
ProfileState::ProfileState() {}
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
	pxUI->vTransitionState(MainMenuState::getInstance());
}


void ProfileState::vEventRight(UI* pxUI) const
{


}


const UIState& ProfileState::getInstance(void)
{
	static const ProfileState xSingleton;
	return xSingleton;
}



// SettingsState
SettingsState::SettingsState() {}
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
	pxUI->vTransitionState(MainMenuState::getInstance());
}


void SettingsState::vEventRight(UI* pxUI) const
{


}


const UIState& SettingsState::getInstance(void)
{
	static const SettingsState xSingleton;
	return xSingleton;
}


// UI
/*
UI::UI() : _xMainMenu(), _xMusic(), _xProfile(), _xSettings(), _pxCurrentState(&_xMainMenu),
		_pxUART(nullptr) {}


UI::UI(UART_HandleTypeDef *pxUART) : _xMainMenu(), _xMusic(), _xProfile(), _xSettings(), _pxCurrentState(&_xMainMenu),
		_pxUART(pxUART)
{
	this->_pxCurrentState->vEnter(this);
}
*/

UI::UI() : _pxCurrentState(&MainMenuState::getInstance()), _pxUART(nullptr) {}


UI::UI(UART_HandleTypeDef *pxUART) : _pxCurrentState(&MainMenuState::getInstance()),_pxUART(pxUART)
{
	this->_pxCurrentState->vEnter(this);
}


UI::~UI()
{
	this->_pxCurrentState->vExit(this);
}


// Main Menu

void UI::MainMenu::vMoveUpStateOption(void)
{
	this->eStateOption = (StateOption)(((int32_t)this->eStateOption + ((int32_t)StateOption::COUNT - 1)) % (int32_t)StateOption::COUNT);
}


void UI::MainMenu::vMoveDownStateOption(void)
{
	this->eStateOption = (StateOption)(((int32_t)this->eStateOption + 1) % (int32_t)StateOption::COUNT);
}



void UI::vUpdate(void)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  //HAL_UART_Transmit(this->_pxUART, (uint8_t *)("update ui\r\n"), sizeof("update ui\r\n"), 100);
	DEBUG_PRINT_STATE();
	DEBUG_PRINT_STATE_OPTION();
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



void UI::vEXTI(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
	case UP_BUTTON:
		HAL_UART_Transmit(this->_pxUART, (uint8_t *)("up\r\n"), sizeof("up\r\n"), 100);
		this->vPressUpButton();
		break;
	case DOWN_BUTTON:
		HAL_UART_Transmit(this->_pxUART, (uint8_t *)("down\r\n"), sizeof("down\r\n"), 100);
		this->vPressDownButton();
		break;
	case LEFT_BUTTON:
		HAL_UART_Transmit(this->_pxUART, (uint8_t *)("left\r\n"), sizeof("left\r\n"), 100);
		this->vPressLeftButton();
		break;
	case RIGHT_BUTTON:
		HAL_UART_Transmit(this->_pxUART, (uint8_t *)("right\r\n"), sizeof("right\r\n"), 100);
		this->vPressRightButton();
		break;
	default:
		break;
	}
}



UART_HandleTypeDef* UI::pxGetUART(void)
{
	return this->_pxUART;
}






