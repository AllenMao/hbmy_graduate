#include "D3DApp.h"
#include "CTerrain.h"
#include "CCamera.h"
#include "CImage.h"
#include "Billboard.h"
#include "PlantCollect.h"
#include "particles.h"
#include "WaterPlane.h"
#include "SkyBox.h"
#include "D3DAudio.h"
#include "Timer.h"

IDirect3DDevice9 *g_pDevice;
LPD3DXFONT g_pTextFPS =NULL;    //字体COM接口

CTerrain *g_pTerrain = NULL;

CD3DApp *g_pD3dApp = NULL;
CCamera *g_pCamera = NULL;
CImage *g_pImage = NULL;

CBillboard *g_pBillboard = NULL;
CPlantCollect *g_pPlant = NULL;
PSys::PSystem* g_pSpring = NULL;
PSys::PSystem* g_pFire = NULL;
SkyBox* g_pSkyBox =NULL;

CAudio *g_pAudio = NULL;
CAudio *g_pVideo = NULL;

CWaterPlane *g_pWater;
CWaterPlane *g_pWater1;
float g_fTotalTime = 0;

CTimer g_pTimer;


float width = 800.0f;
float height =600.0f;



/////////////////////////////////////////////////////////////
void Pre()
{
	g_pTimer.InitTimer();
	//创建字体
	D3DXCreateFont(g_pDevice, 36, 0, 0, 1000, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pTextFPS);

	g_pCamera = new CCamera();
	
	g_pTerrain = new CTerrain(g_pDevice);

	g_pTerrain->MakeTerrainFault(257, 128, 0, 500, 0.4f);
	//g_pTerrain->SaveHeightMap("haha1.raw");
	//g_pTerrain->GenerateTextureMap(256*6);

	//g_pTerrain->LoadTerrainFromFile("heightmap2.raw","ground.jpg","detail.jpg");
	g_pTerrain->LoadTerrainFromFile("haha1.raw","Finish.bmp","detail.jpg");
	//g_pTerrain->LoadTerrainFromFile( "heightmap2.raw", "Finish.bmp","detail.jpg");

	g_pCamera->SetPosX(0.0f);
	g_pCamera->SetPosZ(-100.0f );
	g_pCamera->SetPosY(g_pTerrain->GetTerrainElev( g_pCamera->GetPosX(), g_pCamera->GetPosZ())+50.0f);

	// 设置投影矩阵
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 0.1f, 150000.0f );
	g_pDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	 //设置观察矩阵
    D3DXMATRIXA16 matView;
	D3DXVECTOR3 vEye( g_pCamera->GetPosX(), g_pCamera->GetPosY(), g_pCamera->GetPosZ());
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vEye, &vAt, &vUp );
    g_pDevice->SetTransform( D3DTS_VIEW, &matView );
	
	g_pTerrain->InitQuadTree();
	g_pTerrain->SetDimesion( 1024.0f*1.0f, 1024.0f*1.0f, D3DXVECTOR2(0, 0) );

	if(!SUCCEEDED(g_pTerrain->InitShader()))
		return;
	//sky
	g_pSkyBox = new SkyBox( g_pDevice );
	g_pSkyBox->InitSky( "res\\texture\\Front1.jpg", "res\\texture\\Back1.jpg", "res\\texture\\Left1.jpg", 
		"res\\texture\\Right1.jpg", "res\\texture\\Up1.jpg", "res\\texture\\Down1.jpg", 2000.0f,2000.0f);

	//SECNE
	g_pBillboard = new CBillboard(g_pDevice,D3DXVECTOR3(0.0f , g_pTerrain->GetTerrainElev(0,0) + 30.0f, 0.0f));
	g_pBillboard->Init("tree.dds");

	g_pPlant = new CPlantCollect;
	g_pPlant->Create(50,200,32);

	PSys::BoundingBox boundingBox;
	boundingBox.vMin = D3DXVECTOR3(-550.0f, -10.0f, -550.0f);
	boundingBox.vMax = D3DXVECTOR3( 550.0f,  600.0f,  550.0f);
	g_pSpring = new PSys::Snow(&boundingBox, 2000);
	g_pSpring->init(g_pDevice, "snowflake.dds");

	D3DXVECTOR3 origin(0.0f, 250.0f, 50.0f);
	g_pFire = new PSys::Fire(&origin, 6000);
	g_pFire->init(g_pDevice, "flare.bmp");


	g_pAudio		= new CAudio(false);
	g_pAudio->initMedia("0.mp3");
	g_pAudio->run();
	
	//g_pVideo = new CAudio(true);//弹出窗口视频
	//g_pAudio->initMedia("0.avi");
	//g_pAudio->run();

	/////////////////////Water///////////////////////////////////////
	g_pWater = new CWaterPlane(g_pDevice);
	g_pWater1 = new CWaterPlane(g_pDevice);
	g_pWater->RestoreDeviceObjects(g_pDevice, 200, "water1.bmp");
	//g_pWater->InvalidateDeviceObjects();
}

void d3dRender(float fTimeDelta)
{
	//定义一个矩形，用于获取主窗口矩形
	RECT formatRect;
	GetClientRect(g_pD3dApp->GetHWnd(), &formatRect);
	char _strFPS[50] ={0};    //包含帧速率的字符数组

	/////////////Text for Debug//////////////
	float fTimeDelta1 = fTimeDelta;
	if (fTimeDelta < 0.0001f)
		fTimeDelta1 = 0.0001f;
	g_pWater->Update(fTimeDelta1);
	g_fTotalTime += fTimeDelta;
	if(g_fTotalTime>1.115f)
	{
		g_pWater->CreateWaterDroplet(rand()%200, rand()%200, 10, 2);
		g_fTotalTime = 0;
	}
	g_pAudio->repeat();
	/////////////////////////
	if( GetAsyncKeyState( 'W' ) & 0x8000f )
	{
		g_pCamera->Walk( 0, 5.0f );
	}
	else if( GetAsyncKeyState( 'S' ) & 0x8000f )
	{
		g_pCamera->Walk( 0, -5.0f );
	}

	if( GetAsyncKeyState( 'D' ) & 0x8000f )
	{
		g_pCamera->Walk( 5.0f, 0 );
	}
	else if( GetAsyncKeyState( 'A' ) & 0x8000f )
	{
		g_pCamera->Walk( -5.0f, 0 );
	}
	else if( GetAsyncKeyState( 'Q' ) & 0x8000f )
	{
		
	}
	if( GetAsyncKeyState( VK_UP ) & 0x8000f )
		g_pCamera->Pitch(0.009f);
	if( GetAsyncKeyState( VK_DOWN ) & 0x8000f )
		g_pCamera->Pitch(-0.009f);
	if( GetAsyncKeyState( VK_LEFT ) & 0x8000f )
		g_pCamera->Yaw(-0.009f);
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000f )
		g_pCamera->Yaw(0.009f);

	// 设置世界矩阵
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pDevice->SetTransform( D3DTS_WORLD, &matWorld );
	// 设置视口矩阵
	D3DXMATRIX matView;
	// 根据地形的X，Y坐标来获取地形的高度，再拔高10.0f，即相机的高度
	g_pCamera->SetPosY( g_pTerrain->GetTerrainElev( g_pCamera->GetPosX(), g_pCamera->GetPosZ()) + 50.0f);
	matView = g_pCamera->GetViewMatrix();
	g_pDevice->SetTransform( D3DTS_VIEW, &matView );

	// 创建地形
	g_pTerrain->CreateTerrain(g_pCamera );

	// 设置填充模式
	//g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	// set render state
	//g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	//g_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	//g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	//把填充模式设为线框填充  
	//g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	g_pSpring->update(fTimeDelta);
	g_pFire->update(fTimeDelta);
	if(g_pFire->isDead()) g_pFire->reset();

	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	g_pDevice->BeginScene();

	g_pDevice->SetTransform(D3DTS_WORLD, &I);
	g_pTerrain->RenderTerrain(false);
	g_pSkyBox->Render( &I );

	/*D3DXMatrixTranslation(&I, -1024.0f, 0.0f, 0.0f);
	g_pDevice->SetTransform(D3DTS_WORLD, &I);
	g_pTerrain->RenderTerrain(false);*/

	g_pBillboard->Render();
	g_pPlant->Draw(fTimeDelta);

	//g_pWater->Render();

	g_pSpring->render();
	//g_pFire->render();

	

	D3DXMATRIX matBillboard1;
	D3DXMatrixRotationY( &matBillboard1, D3DX_PI);
	D3DXMatrixIdentity(&I);
	I = I * matBillboard1;
	g_pDevice->SetTransform(D3DTS_WORLD, &I);
	g_pWater1->AnimatingTexture("water1.bmp");

	

	g_pTimer.UpdateFPS();
	//在窗口右上角处，显示每秒帧数
	formatRect.top = 5;
	int charCount = sprintf(_strFPS, "FPS:%0.3f",g_pTimer.GetFPS());
	g_pTextFPS->DrawText(NULL, _strFPS, charCount , &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_RGBA(0,239,136,255));

	g_pDevice->EndScene();
	g_pDevice->Present(0, 0, 0, 0);
}

///////////////////////////////////////////////////
void Shutdown();
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//freopen("myData.txt","w",stdout);
	srand(  (unsigned)GetCurrentTime( ) );
	srand( (unsigned)time( NULL ) );
	HWND hWnd;
	g_pD3dApp = new CD3DApp();
	g_pD3dApp->InitD3D(&g_pDevice,"FPSgame",hInstance,(int)width,(int)height, false);
	hWnd = g_pD3dApp->GetHWnd();

	Pre();

	// 消息循环
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message!=WM_QUIT)
    {
        static FLOAT fLastTime  = (float)::timeGetTime();
        static FLOAT fCurrTime  = (float)::timeGetTime();
        static FLOAT fTimeDelta = 0.0f;
        fCurrTime  = (float)::timeGetTime();
        fTimeDelta = (fCurrTime - fLastTime) / 1000.0f;
        fLastTime  = fCurrTime;
		//g_pTimer.InitTimer();
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			d3dRender(1.0f/g_pTimer.GetFPS());   // 绘制3D场景
        }
		
    }
	Shutdown();
	UnregisterClass("FPSgame",hInstance);
	return true;
}

void Shutdown()
{
	SAFE_RELEASE(g_pDevice);
	SAFE_RELEASE(g_pTextFPS);
	SAFE_DELETE(g_pD3dApp);
	SAFE_DELETE(g_pCamera);
	SAFE_DELETE(g_pImage);
	SAFE_DELETE(g_pBillboard);
	SAFE_DELETE(g_pPlant);
	
	SAFE_DELETE(g_pSpring);
	SAFE_DELETE(g_pSkyBox);
	SAFE_DELETE(g_pAudio);
	SAFE_DELETE(g_pVideo);
	SAFE_DELETE(g_pWater);
	SAFE_DELETE(g_pWater1);
	SAFE_DELETE(g_pTerrain);
}
