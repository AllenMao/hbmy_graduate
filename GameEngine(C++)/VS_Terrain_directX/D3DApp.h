#pragma once
#include "Common.h"

class CD3DApp
{
public:
	CD3DApp(void);
	~CD3DApp(void);
	bool InitD3D(IDirect3DDevice9 **device, LPCTSTR szWinTitle, HINSTANCE hInstance,int width, int height, bool fullscreen = true);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	inline WNDCLASSEX GetWndClass(){return wcex;}
	inline HWND GetHWnd(){return hWnd;}
private:
	HWND hWnd;
	WNDCLASSEX wcex;
};