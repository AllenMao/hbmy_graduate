#include "D3DApp.h"
#include "FPSGame.h"
//#include "Menu.h"
float width = 800.0f;
float height =600.0f;
RECT rect;

/////////////////////////测试区//////////////////////
IDirect3DDevice9 *Device;
HRESULT InitVB();
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;

struct CUSTOMVERTEX
{
	FLOAT x,y,z;
	DWORD color;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
////////////////////////////////////////////////////////

///////////////////测试区//////////////////////////
HRESULT InitVB()
{
	CUSTOMVERTEX vertices[]=
	{
		{-2.0f,-1.0f,0.0f,0xffff0000 },
		{1.0f,-1.0f,0.0f,0xff0000ff },
		{0.0f,1.0f,0.0f,0xffffffff },
	};

	if(FAILED(Device->CreateVertexBuffer(3*sizeof(CUSTOMVERTEX),
											0,D3DFVF_CUSTOMVERTEX,
											D3DPOOL_DEFAULT,&g_pVB,NULL)))
	{
		return E_FAIL;
	}
	VOID* pVertices;
	if(FAILED(g_pVB->Lock(0,sizeof(vertices),(VOID**)&pVertices , 0)))
		return E_FAIL;
	memcpy(pVertices,vertices,sizeof(vertices));
	g_pVB->Unlock();

	return S_OK;
}
void Direct3D_Render( )
{
	D3DXMATRIX g_matWorld;
	D3DXMatrixIdentity(&g_matWorld);

	UINT iTime = timeGetTime() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
	D3DXMatrixRotationY( &g_matWorld , fAngle );
	Device->SetTransform( D3DTS_WORLD, &g_matWorld);

	Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	Device->SetRenderState( D3DRS_LIGHTING, FALSE );
	Device->BeginScene();
	//绘制三角形
	Device->SetStreamSource(0,g_pVB,0,sizeof(CUSTOMVERTEX));
	Device->SetFVF(D3DFVF_CUSTOMVERTEX);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST,0,1);
	Device->EndScene();                       // 结束绘制
	Device->Present(NULL, NULL, NULL, NULL);  // 翻转与显示
}

///////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//freopen( "HeightY.txt", "w", stdout);

	HWND hWnd;
	MSG msg;
	//IDirect3DDevice9 *Device;
	CD3DApp *D3DApp = new CD3DApp;
	D3DApp->InitD3D(&Device,"FPSgame",hInstance,(int)width,(int)height, false);
	hWnd = D3DApp->GetHWnd();
	GetWindowRect(hWnd, &rect);

	FPSGame *fpsgame = new FPSGame(Device, hWnd, hInstance);
	//Menu *menu = new Menu(Device,hWnd);

	InitVB();//调用测试函数

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
		//Direct3D_Render();
		fpsgame->GameLoop(Device);
		
		
	}
	UnregisterClass("FPSgame",hInstance);
	return true;
}