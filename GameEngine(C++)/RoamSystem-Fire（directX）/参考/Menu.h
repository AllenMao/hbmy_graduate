#pragma once

#include "Common.h"
#include "Input.h"

enum GAME_STATE { MENU = 1, START =2};

extern GAME_STATE  gamestate= MENU;

class Menu
{
public:
	Menu(IDirect3DDevice9 *Device,HWND hWnd);
	~Menu();
	void MenuProcess();
	void MenuRender(IDirect3DDevice9 *Device);
	void GUICallback(int id);
private:
	CInput *input;
};