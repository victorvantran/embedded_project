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

	void enter(void);
	void exit(void);

	void eventUp(void);
	void eventDown(void);
	void eventLeft(void);
	void eventRight(void);
};



class MainMenuState : public UIState
{
private:
public:
	MainMenuState();
	~MainMenuState();
};


class UI
{
private:
	UIState _xMainMenu;
	UIState _xMusic;
	UIState _xSettings;

	UART_HandleTypeDef* _pxUART;
public:
	UI();
	UI(UART_HandleTypeDef* pxUART);

	~UI();

	void test(void);
};


#endif /* INC_UI_H_ */
