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


// Forward declaration to resolve circular dependencies
class UI;



class UIState
{
private:
	EnumState _eState;
public:
	UIState(EnumState eState);
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

	void test(void) const;

	inline const UIState* pxGetCurrentState(void) const;

	void vTransitionState(const UIState& xNextState);

	void vPressUpButton(void);
	void vPressDownButton(void);
	void vPressLeftButton(void);
	void vPressRightButton(void);
};


#endif /* INC_UI_H_ */
