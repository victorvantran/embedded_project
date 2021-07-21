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

void MainMenuState::vUpdate(UI* pxUI)
{
	this->DEBUG_PRINT_STATE_OPTION(pxUI);
}


void MainMenuState::vEnter(UI* pxUI)
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("enter: main menu\r\n"), sizeof("enter: main menu\r\n"), 100);
}


void MainMenuState::vExit(UI* pxUI)
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("exit: main menu\r\n"), sizeof("exit: main menu\r\n"), 100);
}


void MainMenuState::vEventUp(UI* pxUI)
{
	pxUI->xGetMainMenuState().vMoveUpStateOption();
}


void MainMenuState::vEventDown(UI* pxUI)
{
	pxUI->xGetMainMenuState().vMoveDownStateOption();
}


void MainMenuState::vEventLeft(UI* pxUI)
{
	// NOP
}


void MainMenuState::vEventRight(UI* pxUI)
{
	switch (this->_eStateOption)
	{
	case StateOption::MUSIC:
		pxUI->vTransitionState(pxUI->xGetMusicState());
	break;
	case StateOption::PROFILE:
		pxUI->vTransitionState(pxUI->xGetProfileState());
	break;
	case StateOption::SETTINGS:
		pxUI->vTransitionState(pxUI->xGetSettingsState());
	break;
	default:
	break;
	}
}


void MainMenuState::vMoveUpStateOption(void)
{
	this->_eStateOption = (StateOption)(((int32_t)this->_eStateOption + ((int32_t)StateOption::COUNT - 1)) % (int32_t)StateOption::COUNT);
}


void MainMenuState::vMoveDownStateOption(void)
{
	this->_eStateOption = (StateOption)(((int32_t)this->_eStateOption + 1) % (int32_t)StateOption::COUNT);
}




void MainMenuState::DEBUG_PRINT_STATE_OPTION(UI* pxUI)
{
	switch (this->_eStateOption)
	{
	case StateOption::MUSIC:
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("-> music\r\n"), 	 	sizeof("-> music\r\n"), 		100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   profile\r\n"), 	sizeof("   profile\r\n"), 	100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   settings\r\n"),	sizeof("   settings\r\n"), 100);
		break;
	case StateOption::PROFILE:
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   music\r\n"), 	 	sizeof("   music\r\n"), 		100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("-> profile\r\n"), 	sizeof("-> profile\r\n"), 	100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   settings\r\n"),	sizeof("   settings\r\n"), 100);
		break;
	case StateOption::SETTINGS:
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   music\r\n"), 	 	sizeof("   music\r\n"), 		100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   profile\r\n"), 	sizeof("   profile\r\n"), 	100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("-> settings\r\n"),	sizeof("-> settings\r\n"), 100);
		break;
	default:
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("idk\r\n"), sizeof("idk\r\n"), 100);
		break;
	}
}




// MusicState
MusicState::MusicState() {}
MusicState::~MusicState() {}

void MusicState::vUpdate(UI* pxUI)
{

}

void MusicState::vEnter(UI* pxUI)
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("enter: music\r\n"), sizeof("enter: music\r\n"), 100);
}


void MusicState::vExit(UI* pxUI)
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("exit: music\r\n"), sizeof("exit: music\r\n"), 100);
}


void MusicState::vEventUp(UI* pxUI)
{

}


void MusicState::vEventDown(UI* pxUI)
{

}


void MusicState::vEventLeft(UI* pxUI)
{
	pxUI->vTransitionState(pxUI->xGetMainMenuState());
}


void MusicState::vEventRight(UI* pxUI)
{


}



// ProfileState
ProfileState::ProfileState() {}
ProfileState::~ProfileState() {}

void ProfileState::vUpdate(UI* pxUI)
{

}

void ProfileState::vEnter(UI* pxUI)
{

}


void ProfileState::vExit(UI* pxUI)
{

}


void ProfileState::vEventUp(UI* pxUI)
{

}


void ProfileState::vEventDown(UI* pxUI)
{

}


void ProfileState::vEventLeft(UI* pxUI)
{
	pxUI->vTransitionState(pxUI->xGetMainMenuState());
}


void ProfileState::vEventRight(UI* pxUI)
{


}



// SettingsState
SettingsState::SettingsState() {}
SettingsState::~SettingsState() {}

void SettingsState::vUpdate(UI* pxUI)
{

}

void SettingsState::vEnter(UI* pxUI)
{

}


void SettingsState::vExit(UI* pxUI)
{

}


void SettingsState::vEventUp(UI* pxUI)
{

}


void SettingsState::vEventDown(UI* pxUI)
{

}


void SettingsState::vEventLeft(UI* pxUI)
{
	pxUI->vTransitionState(pxUI->xGetMainMenuState());
}


void SettingsState::vEventRight(UI* pxUI)
{


}



// UI
UI::UI() : _xMainMenu(), _xMusic(), _xProfile(), _xSettings(), _pxCurrentState(&this->_xMainMenu),
		_pxUART(nullptr) {}


UI::UI(UART_HandleTypeDef *pxUART) : _xMainMenu(), _xMusic(), _xProfile(), _xSettings(), _pxCurrentState(&this->_xMainMenu),
		_pxUART(pxUART)
{
	this->_pxCurrentState->vEnter(this);
}




UI::~UI()
{
	this->_pxCurrentState->vExit(this);
}





void UI::vUpdate(void)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  //HAL_UART_Transmit(this->_pxUART, (uint8_t *)("update ui\r\n"), sizeof("update ui\r\n"), 100);
	this->_pxCurrentState->vUpdate(this);
	DEBUG_PRINT_STATE();
}


UIState* UI::pxGetCurrentState(void)
{
	return this->_pxCurrentState;
}


void UI::vTransitionState(UIState& xNextState)
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



MainMenuState& UI::xGetMainMenuState(void)
{
	return this->_xMainMenu;
}

MusicState& UI::xGetMusicState(void)
{
	return this->_xMusic;
}

ProfileState& UI::xGetProfileState(void)
{
	return this->_xProfile;
}

SettingsState& UI::xGetSettingsState(void)
{
	return this->_xSettings;
}

UART_HandleTypeDef* UI::pxGetUART(void)
{
	return this->_pxUART;
}






