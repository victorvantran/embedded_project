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


enum class StateOption
{
	MUSIC,
	PROFILE,
	SETTINGS,
	COUNT
};


// Forward declaration to appease circular dependency
class UI;


class UIState
{
private:
public:
	UIState();
	virtual ~UIState();

	virtual void vEnter(UI* pxUI) const = 0;
	virtual void vExit(UI* pxUI) const = 0;

	virtual void vEventUp(UI* pxUI) const = 0;
	virtual void vEventDown(UI* pxUI) const = 0;
	virtual void vEventLeft(UI* pxUI) const = 0;
	virtual void vEventRight(UI* pxUI) const = 0;
};



class MainMenuState : public UIState
{
private:
public:
	MainMenuState();
	virtual ~MainMenuState();

	virtual void vEnter(UI* pxUI) const;
	virtual void vExit(UI* pxUI) const;

	virtual void vEventUp(UI* pxUI) const;
	virtual void vEventDown(UI* pxUI) const;
	virtual void vEventLeft(UI* pxUI) const;
	virtual void vEventRight(UI* pxUI) const;

	static const UIState& getInstance(void);
};


class MusicState : public UIState
{
private:
public:
	MusicState();
	virtual ~MusicState();

	virtual void vEnter(UI* pxUI) const;
	virtual void vExit(UI* pxUI) const;

	virtual void vEventUp(UI* pxUI) const;
	virtual void vEventDown(UI* pxUI) const;
	virtual void vEventLeft(UI* pxUI) const;
	virtual void vEventRight(UI* pxUI) const;

	static const UIState& getInstance(void);
};


class ProfileState : public UIState
{
private:
public:
	ProfileState();
	virtual ~ProfileState();

	virtual void vEnter(UI* pxUI) const;
	virtual void vExit(UI* pxUI) const;

	virtual void vEventUp(UI* pxUI) const;
	virtual void vEventDown(UI* pxUI) const;
	virtual void vEventLeft(UI* pxUI) const;
	virtual void vEventRight(UI* pxUI) const;

	static const UIState& getInstance(void);
};



class SettingsState : public UIState
{
private:
public:
	SettingsState();
	virtual ~SettingsState();

	virtual void vEnter(UI* pxUI) const;
	virtual void vExit(UI* pxUI) const;

	virtual void vEventUp(UI* pxUI) const;
	virtual void vEventDown(UI* pxUI) const;
	virtual void vEventLeft(UI* pxUI) const;
	virtual void vEventRight(UI* pxUI) const;

	static const UIState& getInstance(void);
};



class UI
{
private:
	/*
	 * Non-singleton pattern
	const MainMenuState _xMainMenu;
	const MusicState _xMusic;
	const ProfileState _xProfile;
	const SettingsState _xSettings;
	*/
	const UIState* _pxCurrentState;
	UART_HandleTypeDef* _pxUART;


public:
	UI();
	UI(UART_HandleTypeDef* pxUART);

	~UI();



	void DEBUG_PRINT_STATE(void)
	{

		if (this->_pxCurrentState == &MainMenuState::getInstance())
		{
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("MAIN_MENU\r\n"), sizeof("MAIN_MENU\r\n"), 100);
		}
		else if (this->_pxCurrentState == &MusicState::getInstance())
		{
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("MUSIC\r\n"), sizeof("MUSIC\r\n"), 100);
		}
		else if (this->_pxCurrentState == &ProfileState::getInstance())
		{
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("PROFILE\r\n"), sizeof("PROFILE\r\n"), 100);
		}
		else if (this->_pxCurrentState == &SettingsState::getInstance())
		{
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("SETTINGS\r\n"), sizeof("SETTINGS\r\n"), 100);
		}
		else
		{

		}

	}


	// Main menu
	struct MainMenu
	{
		StateOption eStateOption = StateOption::MUSIC;

		void vMoveUpStateOption(void);
		void vMoveDownStateOption(void);
	} xMainMenu;

	void DEBUG_PRINT_STATE_OPTION(void)
	{
		switch (this->xMainMenu.eStateOption)
		{
		case StateOption::MUSIC:
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("-> music\r\n"), sizeof("-> music\r\n"), 100);
			break;
		case StateOption::PROFILE:
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("-> profile\r\n"), sizeof("-> profile\r\n"), 100);
			break;
		case StateOption::SETTINGS:
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("-> settings\r\n"), sizeof("-> settings\r\n"), 100);
			break;
		default:
		  HAL_UART_Transmit(this->_pxUART, (uint8_t *)("idk\r\n"), sizeof("idk\r\n"), 100);
			break;
		}
	}


	// Music
	struct Music
	{

	} xMusic;



	// Profile
	struct Profile
	{



	} xProfile;



	// Settings
	struct Settings
	{

	} xSettings;




	void vUpdate(void);

	const UIState* pxGetCurrentState(void) const;

	void vTransitionState(const UIState& xNextState);

	void vPressUpButton(void);
	void vPressDownButton(void);
	void vPressLeftButton(void);
	void vPressRightButton(void);


	void vEXTI(uint16_t GPIO_Pin);

	// Getters
	UART_HandleTypeDef* pxGetUART(void);
};







#endif /* INC_UI_H_ */
