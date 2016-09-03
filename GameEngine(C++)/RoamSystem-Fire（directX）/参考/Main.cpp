#include "D3DApp.h"
#include "FPSGame.h"
#include "Menu.h"
float width = 800.0f;
float height =600.0f;
RECT rect;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	HWND hWnd;
	MSG msg;
	IDirect3DDevice9 *Device;
	CD3DApp *D3DApp = new CD3DApp;
	D3DApp->InitD3D(&Device,"FPSgame",hInstance,(int)width,(int)height, false);
	hWnd = D3DApp->GetHWnd();
	GetWindowRect(hWnd, &rect);
	FPSGame *fpsgame = new FPSGame(Device, hWnd, hInstance);
	//Menu *menu = new Menu(Device,hWnd);
	while(true)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//switch(GAME_STATE)
		//case 1:
		//menu->process();
		//case 2:
		fpsgame->GameLoop(Device);
	}
	UnregisterClass("FPSgame",hInstance);
	return true;
}