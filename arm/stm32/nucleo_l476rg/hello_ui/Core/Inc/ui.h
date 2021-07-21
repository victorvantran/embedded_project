/*
 * ui.h
 *
 *  Created on: Jul 12, 2021
 *      Author: victor
 */

#ifndef INC_UI_H_
#define INC_UI_H_

#include "stm32l4xx_hal.h"



#define UP_BUTTON			GPIO_PIN_15
#define DOWN_BUTTON 	GPIO_PIN_14
#define LEFT_BUTTON 	GPIO_PIN_13
#define RIGHT_BUTTON 	GPIO_PIN_12




// Forward declaration to appease circular dependency
class UI;


class UIState
{
private:
public:
	UIState();
	virtual ~UIState();

	virtual void vUpdate(UI* pxUI) = 0;

	virtual void vEnter(UI* pxUI) = 0;
	virtual void vExit(UI* pxUI) = 0;

	virtual void vEventUp(UI* pxUI) = 0;
	virtual void vEventDown(UI* pxUI) = 0;
	virtual void vEventLeft(UI* pxUI) = 0;
	virtual void vEventRight(UI* pxUI) = 0;
};



class MainMenuState : public UIState
{
private:
	enum class StateOption
	{
		MUSIC_LIST,
		PROFILE,
		SETTINGS,
		COUNT
	};

	MainMenuState::StateOption _eStateOption = MainMenuState::StateOption::MUSIC_LIST;
public:
	MainMenuState();
	virtual ~MainMenuState();

	virtual void vUpdate(UI* pxUI);

	virtual void vEnter(UI* pxUI);
	virtual void vExit(UI* pxUI);

	virtual void vEventUp(UI* pxUI);
	virtual void vEventDown(UI* pxUI);
	virtual void vEventLeft(UI* pxUI);
	virtual void vEventRight(UI* pxUI);

	void vMoveUpStateOption(void);
	void vMoveDownStateOption(void);


	// Debug
	void DEBUG_PRINT(UI* pxUI);
};


class MusicListState : public UIState
{
private:

public:
	MusicListState();
	virtual ~MusicListState();

	virtual void vUpdate(UI* pxUI);

	virtual void vEnter(UI* pxUI);
	virtual void vExit(UI* pxUI);

	virtual void vEventUp(UI* pxUI);
	virtual void vEventDown(UI* pxUI);
	virtual void vEventLeft(UI* pxUI);
	virtual void vEventRight(UI* pxUI);

	// Debug
	void DEBUG_PRINT(UI* pxUI);
};


class ProfileState : public UIState
{
private:
public:
	ProfileState();
	virtual ~ProfileState();

	virtual void vUpdate(UI* pxUI);

	virtual void vEnter(UI* pxUI);
	virtual void vExit(UI* pxUI);

	virtual void vEventUp(UI* pxUI);
	virtual void vEventDown(UI* pxUI);
	virtual void vEventLeft(UI* pxUI);
	virtual void vEventRight(UI* pxUI);

	// Debug
	void DEBUG_PRINT(UI* pxUI);
};



class SettingsState : public UIState
{
private:
public:
	SettingsState();
	virtual ~SettingsState();

	virtual void vUpdate(UI* pxUI);

	virtual void vEnter(UI* pxUI);
	virtual void vExit(UI* pxUI);

	virtual void vEventUp(UI* pxUI);
	virtual void vEventDown(UI* pxUI);
	virtual void vEventLeft(UI* pxUI);
	virtual void vEventRight(UI* pxUI);

	// Debug
	void DEBUG_PRINT(UI* pxUI);
};




class ModeState : public UIState
{
private:

public:
	ModeState();
	virtual ~ModeState();

	virtual void vUpdate(UI* pxUI);

	virtual void vEnter(UI* pxUI);
	virtual void vExit(UI* pxUI);

	virtual void vEventUp(UI* pxUI);
	virtual void vEventDown(UI* pxUI);
	virtual void vEventLeft(UI* pxUI);
	virtual void vEventRight(UI* pxUI);

	// Debug
	void DEBUG_PRINT(UI* pxUI);
};




class UI
{
private:
	MainMenuState _xMainMenu;
	MusicListState _xMusicList;
	ProfileState _xProfile;
	SettingsState _xSettings;

	UIState* _pxCurrentState;
	UART_HandleTypeDef* _pxUART;


public:
	UI();
	UI(UART_HandleTypeDef* pxUART);

	~UI();


	// Getters
  MainMenuState& xGetMainMenuState(void);
  MusicListState& xGetMusicListState(void);
	ProfileState& xGetProfileState(void);
	SettingsState& xGetSettingsState(void);

	UART_HandleTypeDef* pxGetUART(void);


	// Debug
	void DEBUG_PRINT_STATE(void)
	{
		if (this->_pxCurrentState == &this->_xMainMenu)
		{
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("MAIN_MENU\r\n"), sizeof("MAIN_MENU\r\n"), 100);
		}
		else if (this->_pxCurrentState == &this->_xMusicList)
		{
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("MUSIC_LIST\r\n"), sizeof("MUSIC_LIST\r\n"), 100);
		}
		else if (this->_pxCurrentState == &this->_xProfile)
		{
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("PROFILE\r\n"), sizeof("PROFILE\r\n"), 100);
		}
		else if (this->_pxCurrentState == &this->_xSettings)
		{
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("SETTINGS\r\n"), sizeof("SETTINGS\r\n"), 100);
		}
		else
		{

		}

	}



	void vUpdate(void);

	UIState* pxGetCurrentState(void);

	void vTransitionState(UIState& xNextState);

	void vPressUpButton(void);
	void vPressDownButton(void);
	void vPressLeftButton(void);
	void vPressRightButton(void);


	void vEXTI(uint16_t GPIO_Pin);


};







#endif /* INC_UI_H_ */
