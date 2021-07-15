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
	PROFILE,
	SETTINGS,
	COUNT
};



class UIState
{
private:
	EnumState _eState;
public:
	UIState(EnumState eState);
	virtual ~UIState();

	virtual void vEnter(void) = 0;
	virtual void vExit(void) = 0;

	virtual void vEventUp(void) = 0;
	virtual void vEventDown(void) = 0;
	virtual void vEventLeft(void) = 0;
	virtual void vEventRight(void) = 0;
};



class MainMenuState : public UIState
{
private:
public:
	MainMenuState();
	virtual ~MainMenuState();

	virtual void vEnter(void);
	virtual void vExit(void);

	virtual void vEventUp(void);
	virtual void vEventDown(void);
	virtual void vEventLeft(void);
	virtual void vEventRight(void);
};


class MusicState : public UIState
{
private:
public:
	MusicState();
	virtual ~MusicState();

	virtual void vEnter(void);
	virtual void vExit(void);

	virtual void vEventUp(void);
	virtual void vEventDown(void);
	virtual void vEventLeft(void);
	virtual void vEventRight(void);
};


class ProfileState : public UIState
{
private:
public:
	ProfileState();
	virtual ~ProfileState();

	virtual void vEnter(void);
	virtual void vExit(void);

	virtual void vEventUp(void);
	virtual void vEventDown(void);
	virtual void vEventLeft(void);
	virtual void vEventRight(void);
};



class SettingsState : public UIState
{
private:
public:
	SettingsState();
	virtual ~SettingsState();

	virtual void vEnter(void);
	virtual void vExit(void);

	virtual void vEventUp(void);
	virtual void vEventDown(void);
	virtual void vEventLeft(void);
	virtual void vEventRight(void);
};



class UI
{
private:
	MainMenuState _xMainMenu;
	MusicState _xMusic;
	ProfileState _xProfile;
	SettingsState _xSettings;
	UIState *_pxCurrState;

	UART_HandleTypeDef* _pxUART;
public:
	UI();
	UI(UART_HandleTypeDef* pxUART);

	~UI();

	void test(void);
};


#endif /* INC_UI_H_ */
