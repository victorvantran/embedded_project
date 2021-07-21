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
	this->DEBUG_PRINT(pxUI);
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
	case MainMenuState::StateOption::MUSIC_LIST:
		pxUI->vTransitionState(pxUI->xGetMusicListState());
	break;
	case MainMenuState::StateOption::PROFILE:
		pxUI->vTransitionState(pxUI->xGetProfileState());
	break;
	case MainMenuState::StateOption::SETTINGS:
		pxUI->vTransitionState(pxUI->xGetSettingsState());
	break;
	default:
	break;
	}
}


void MainMenuState::vMoveUpStateOption(void)
{
	this->_eStateOption = (MainMenuState::StateOption)(((int32_t)this->_eStateOption + ((int32_t)MainMenuState::StateOption::COUNT - 1)) % (int32_t)MainMenuState::StateOption::COUNT);
}


void MainMenuState::vMoveDownStateOption(void)
{
	this->_eStateOption = (MainMenuState::StateOption)(((int32_t)this->_eStateOption + 1) % (int32_t)MainMenuState::StateOption::COUNT);
}




void MainMenuState::DEBUG_PRINT(UI* pxUI)
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("[Main Menu]\r\n"), sizeof("[Main Menu]\r\n"), 100);

	switch (this->_eStateOption)
	{
	case MainMenuState::StateOption::MUSIC_LIST:
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("-> music\r\n"), 	 	sizeof("-> music\r\n"), 		100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   profile\r\n"), 	sizeof("   profile\r\n"), 	100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   settings\r\n"),	sizeof("   settings\r\n"), 100);
		break;
	case MainMenuState::StateOption::PROFILE:
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   music\r\n"), 	 	sizeof("   music\r\n"), 		100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("-> profile\r\n"), 	sizeof("-> profile\r\n"), 	100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   settings\r\n"),	sizeof("   settings\r\n"), 100);
		break;
	case MainMenuState::StateOption::SETTINGS:
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   music\r\n"), 	 	sizeof("   music\r\n"), 		100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("   profile\r\n"), 	sizeof("   profile\r\n"), 	100);
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("-> settings\r\n"),	sizeof("-> settings\r\n"), 100);
		break;
	default:
		HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("idk\r\n"), sizeof("idk\r\n"), 100);
		break;
	}
}




// MusicListState
MusicListState::MusicListState() {}
MusicListState::~MusicListState() {}

void MusicListState::vUpdate(UI* pxUI)
{
	this->DEBUG_PRINT(pxUI);
}


void MusicListState::vEnter(UI* pxUI)
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("enter: music\r\n"), sizeof("enter: music\r\n"), 100);
}


void MusicListState::vExit(UI* pxUI)
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("exit: music\r\n"), sizeof("exit: music\r\n"), 100);
}


void MusicListState::vEventUp(UI* pxUI)
{

}


void MusicListState::vEventDown(UI* pxUI)
{

}


void MusicListState::vEventLeft(UI* pxUI)
{
	pxUI->vTransitionState(pxUI->xGetMainMenuState());
}


void MusicListState::vEventRight(UI* pxUI)
{


}


void MusicListState::DEBUG_PRINT(UI* pxUI)
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("[MusicList]\r\n"), sizeof("[MusicList]\r\n"), 100);
}



// ProfileState
ProfileState::ProfileState() {}
ProfileState::~ProfileState() {}

void ProfileState::vUpdate(UI* pxUI)
{
	this->DEBUG_PRINT(pxUI);
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


void ProfileState::DEBUG_PRINT(UI* pxUI)
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("[Profile]\r\n"), sizeof("[Profile]\r\n"), 100);
}



// SettingsState
SettingsState::SettingsState() {}
SettingsState::~SettingsState() {}

void SettingsState::vUpdate(UI* pxUI)
{
	this->DEBUG_PRINT(pxUI);
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


void SettingsState::DEBUG_PRINT(UI* pxUI)
{
  HAL_UART_Transmit(pxUI->pxGetUART(), (uint8_t *)("[Settings]\r\n"), sizeof("[Settings]\r\n"), 100);
}


// UI
UI::UI() : _xMainMenu(), _xMusicList(), _xProfile(), _xSettings(), _pxCurrentState(&this->_xMainMenu),
		_pxUART(nullptr) {}


UI::UI(UART_HandleTypeDef *pxUART) : _xMainMenu(), _xMusicList(), _xProfile(), _xSettings(), _pxCurrentState(&this->_xMainMenu),
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

MusicListState& UI::xGetMusicListState(void)
{
	return this->_xMusicList;
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






