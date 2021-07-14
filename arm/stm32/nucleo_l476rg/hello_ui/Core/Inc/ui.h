/*
 * ui.h
 *
 *  Created on: Jul 12, 2021
 *      Author: victor
 */

#ifndef INC_UI_H_
#define INC_UI_H_

#include "stm32l4xx_hal.h"


enum class EnumState
{
	MAIN_MENU,
	MUSIC,
	SETTINGS
};



class UIState
{
private:
	EnumState _eState;
public:
	UIState(EnumState eState);
	~UIState();
};



class MainMenuState : public UIState
{
private:
public:
	MainMenuState();
	~MainMenuState();

	void enter(void);
	void exit(void);

	void eventUp(void);
	void eventDown(void);
	void eventLeft(void);
	void eventRight(void);
};


class MusicState : public UIState
{
private:
public:
	MusicState();
	~MusicState();

	void enter(void);
	void exit(void);

	void eventUp(void);
	void eventDown(void);
	void eventLeft(void);
	void eventRight(void);
};


class SettingsState : public UIState
{
private:
public:
	SettingsState();
	~SettingsState();

	void enter(void);
	void exit(void);

	void eventUp(void);
	void eventDown(void);
	void eventLeft(void);
	void eventRight(void);
};



class UI
{
private:
	MainMenuState _xMainMenu;
	MusicState _xMusic;
	SettingsState _xSettings;

	UART_HandleTypeDef* _pxUART;
public:
	UI();
	UI(UART_HandleTypeDef* pxUART);

	~UI();

	void test(void);
};


#endif /* INC_UI_H_ */
