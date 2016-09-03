#include "D3DApp.h"

CD3DApp::CD3DApp(void)
{
}

CD3DApp::~CD3DApp(void)
{
}

bool CD3DApp::InitD3D(IDirect3DDevice9 **device, LPCTSTR szWinTitle, HINSTANCE hInstance,int width, int height, bool fullscreen)
{
	RegWndClass(hInstance);
	hWnd = CreateWindow("game", 
						szWinTitle,
						WS_OVERLAPPEDWINDOW,
						0,
						0,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						NULL,
						NULL,
						hInstance,
						NULL);
	if(!hWnd)
	{
		return false;
	}
	ShowCursor(false);//Òþ²ØÊó±ê
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	IDirect3D9 *d3d9 = NULL;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if(d3d9 == NULL)
	{
		return false;
	}
	D3DCAPS9 caps;
	if(FAILED(d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps)))
	{
		d3d9->Release();
		return false;
	}
	int vp = 0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	D3DPRESENT_PARAMETERS dpp;
	dpp.BackBufferCount = 1;
	dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	dpp.BackBufferHeight = height;
	dpp.BackBufferWidth = width;
	dpp.EnableAutoDepthStencil = true;
	dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
	dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	dpp.Flags = 0;
	dpp.hDeviceWindow = hWnd;
	dpp.Windowed = !fullscreen;
	dpp.MultiSampleQuality = 0;
	dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	
	if(FAILED( d3d9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,vp,&dpp,device ) ) )
	{
		d3d9->Release();
		return false;
	}
	d3d9->Release();
	return true;
}

LRESULT CALLBACK CD3DApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int b=3;
	int &a= b;
	switch (message) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM CD3DApp::RegWndClass(HINSTANCE hInstance)
{
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(NULL, IDI_INFORMATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "game";
	wcex.hIconSm		= LoadIcon(wcex.hInstance, NULL);
	return RegisterClassEx(&wcex);
}