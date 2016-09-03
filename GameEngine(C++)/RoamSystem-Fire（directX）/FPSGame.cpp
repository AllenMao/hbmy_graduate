#include "FPSgame.h"
extern RECT rect;


bool FPSGame::ray_intersect_AABB(D3DXVECTOR3 pos,D3DXVECTOR3 dir,D3DXVECTOR3 AABBmin,D3DXVECTOR3 AABBmax,float &hitdistance)
{
	float lowt = 0.0f; //用于保存最近的交点
	float t;
	bool hit = false;
	D3DXVECTOR3 hitpoint;
	D3DXVECTOR3 min = AABBmin;
	D3DXVECTOR3 max = AABBmax;
	D3DXVECTOR3 rayorig = pos;
	D3DXVECTOR3 raydir = dir;

	//先检查在盒子内
	if(rayorig > min && rayorig < max)
	{
		hitdistance = 0;
		return true;
	}

	//依次检查各面的相交情况
	if(rayorig.x < min.x && raydir.x > 0)
	{
		t = (min.x - rayorig.x)/raydir.x; //利用射线与平面相交的交点公式，先求出交点，

		//然后判断交点是否在当前面内
		if(t>0)
		{
			hitpoint = rayorig + raydir * t;
			if(hitpoint.y>=min.y && hitpoint.y<=max.y && hitpoint.z>=min.z && hitpoint.z<=max.z && (!hit || t<lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}

	if(rayorig.x > max.x && raydir.x < 0)
	{
		t = (max.x - rayorig.x)/raydir.x;
		if(t>0)
		{
			hitpoint = rayorig + raydir*t;
			if(hitpoint.y>min.y && hitpoint.y<=max.y &&
				hitpoint.z>=min.z && hitpoint.z<=max.z &&
				(!hit || t<lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}

	if(rayorig.y<min.y && raydir.y>0)
	{
		t = (min.y-rayorig.y)/raydir.y;
		if(t>0)
		{
			hitpoint = rayorig + raydir*t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}

	if (rayorig.y > max.y && raydir.y < 0)
	{
		t = (max.y - rayorig.y) / raydir.y;
		if (t > 0)
		{
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}

	if (rayorig.z < min.z && raydir.z > 0)
	{
		t = (min.z - rayorig.z) / raydir.z;
		if (t > 0)
		{
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}

	if (rayorig.z > max.z && raydir.z < 0)
	{
		t = (max.z - rayorig.z) / raydir.z;
		if (t > 0)
		{
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	} 

	hitdistance = lowt;
	return hit;
}



FPSGame::FPSGame(IDirect3DDevice9 *Device, HWND hWnd, HINSTANCE hInstance)
{
	Timer = new CTimer();
	//Timer->InitTimer();
	time = Timer->GetPlayTime();

	CycleTimer1 = 0;
	CycleTimer4 = 0;
	soundcycle4 = 0.2f;
	CycleTimer3 =0;
	soundcycle3 = 0.04f;
	waitTimer =0.0f;

	Camera = new CCamera( Device );
	Material = new CMaterial;
	LightforTerrain = new CLight;

	SoundManager = new CSoundManager;
	SoundManager->Init(hWnd);
	SoundManager->SetPrimaryBufferFormat();

	Sound1 = new CSound;
	Sound2 = new CSound;
	Sound3 = new CSound;
	Sound4 = new CSound;
	Sound5 = new CSound;

	Sound1->LoadWaveFile(_T("res/sound/pl_step3.wav"));
	Sound1->SetVolume(10);
	Sound2->LoadWaveFile(_T("res/sound/BGM.wav"));
	Sound4->LoadWaveFile(_T("res/sound/bossattack.wav"));
	Sound2->Play(true);
	Sound3->LoadWaveFile(_T("res/sound/fire.wav"));

	Sound5->LoadWaveFile(_T("res/sound/over.wav"));

	Input = new CInput(hWnd);

	terrain = new CTerrain( Device );   
	terrain->LoadTerrainFromFile( "res\\texture\\heightmap.raw", "res\\texture\\Terrain_grass.tga", "res\\texture\\detail.jpg" );		//从文件加载高度图和纹理
	terrain->InitTerrain( 256, 256, 40.0f, 0.8f );  //四个值分别是顶点行数，顶点列数，顶点间间距，缩放系数

	//初始字体
	D3DXFONT_DESC font_desc;													
	font_desc.Height = 20;
	font_desc.Width = 10;
	font_desc.Weight = 500;
	font_desc.MipLevels = 0;
	font_desc.Italic = false;
	font_desc.CharSet = DEFAULT_CHARSET;
	font_desc.OutputPrecision = 0;
	font_desc.PitchAndFamily = 0;
	font_desc.Quality = 0;
	char *a = "黑体";
	strcpy(font_desc.FaceName, a);
	D3DXCreateFontIndirect(Device, &font_desc, &font);

	//初始天空盒子
	Device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50,50,50));
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRRORONCE);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRRORONCE);
	skybox = new SkyBox( Device );
	skybox->InitSky( "res\\texture\\Front1.jpg", "res\\texture\\Back1.jpg", "res\\texture\\Left1.jpg", 
		"res\\texture\\Right1.jpg", "res\\texture\\Up1.jpg", "res\\texture\\Down1.jpg", 8000.0f, 6500.0f);

	//初始枪
	mygun.Init(Device);
	//敌人
	enemy = new Enemy(Device);
	enemy->Init(Device,terrain);

}

FPSGame::~FPSGame(void)
{
}

void FPSGame:: Direct3D_Update( )
{
	//限制声音频率1
	if(CycleTimer1 >soundcycle1)
	{
		Sound1ready = true;
		CycleTimer1 = 0;
	}
	else
	{
		CycleTimer1 += time;
		Sound1ready = false;
	}
	//限制声音频率2
	if(CycleTimer4 >soundcycle4)
	{
		Sound4ready = true;
		CycleTimer4 = 0;
	}
	else
	{
		CycleTimer4 += time;
		Sound4ready = false;
	}

	if(CycleTimer3 >soundcycle3 * 0.1f)
	{
		Sound3ready = true;
		CycleTimer3 = 0;
	}
	else
	{
		CycleTimer3 += time;
		Sound3ready = false;
	}

	if(enemydead)
	{
		if( waitTimer > 0.8f){
			renderstop = true;
		}
		else
		{
			waitTimer += time;
			renderstop = false;
		}
	}
	if(GetAsyncKeyState(VK_ESCAPE) & 0x8000f)
	{
		PostQuitMessage(0);
	}
	if( GetAsyncKeyState( 'W' ) & 0x8000f )
	{
		Camera->Walk( 1.0f );
	}
	else if( GetAsyncKeyState( 'S' ) & 0x8000f )
	{
		Camera->Walk( -1.0f );
	}
	if( GetAsyncKeyState( 'A' ) & 0x8000f )
	{
		Camera->Strafe(-1.0f);
	}
	if( GetAsyncKeyState( 'D' ) & 0x8000f )
	{
		Camera->Strafe(1.0f);
	}
	if( GetAsyncKeyState( VK_UP ) & 0x8000f )
		Camera->Pitch(0.003f);
	if( GetAsyncKeyState( VK_DOWN ) & 0x8000f )
		Camera->Pitch(-0.003f);
	if( GetAsyncKeyState( VK_LEFT ) & 0x8000f )
		Camera->Yaw(-0.003f);
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000f )
		Camera->Yaw(0.003f);

	if( GetAsyncKeyState( 'J' ) & 0x8000f )
	{
		mygun.Fire();
		if(Sound3ready)
		{
			Sound3->Play(false);
		}
	}
	else mygun.Fired();
}
void FPSGame::GameLoop(IDirect3DDevice9 *Device)
{
	//帧频
	Timer->UpdateFPS();
	//////////////////////////////////////////////////////////////////////////开始渲染////////////////////////////////////////////////////////////////////////////////////
	Device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0 );
	Device->BeginScene();

	Direct3D_Update( );


	////设置灯光
	LightforTerrain->SetDirectionLight(Device, D3DXVECTOR3(-0.8f, -1.2f, -1.0f), D3DCOLOR_XRGB(255,255,255));//作用不知道
	Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );


	//设置摄像机高度和天空位置
	D3DXMATRIX skyboxMatrix;
	D3DXVECTOR3 camerapos;
	Camera->GetPos(&camerapos);
	//限制移动，不要超出terrain
	if(camerapos.x > 4000 ) camerapos.x =4000;
	if(camerapos.x <-4000 ) camerapos.x =-4000;
	if(camerapos.z > 4000 ) camerapos.z =4000;
	if(camerapos.z <-4000 ) camerapos.z =-4000;

	D3DXMatrixTranslation( &skyboxMatrix, camerapos.x, -3200.0f, camerapos.z );
	float terrainheight = terrain->GetCameraHeight(camerapos.x,camerapos.z);

	camerapos.y = terrainheight + 80.0f;

	Camera->SetPos((camerapos));

	//摆放摄像机
	Camera->SetCamera( (float)rect.right, (float)rect.bottom );
	D3DXMATRIX V = Camera->GetMatrix();
	Device->SetTransform(D3DTS_VIEW, &V);

	//设置材质  作用未知？
	Material->SetAmbientColor(201,100,20,0);
	Material->SetDiffuseColor(150,90,10,0);
	Material->SetEmissiveColor(15,15,15,50);
	Material->SetPower(0.1);
	Device->SetMaterial(Material->GetMaterial());

	skybox->Render( &skyboxMatrix );

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, 0 );
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, 0 );//取消不了？

	terrain->Render(Device);

	//更新敌人
	if(enemy->GetHP()>0)
	{
		enemy->Move(Camera,terrain);
	}
	else
	{
		enemy->Die();
	}

	//更新枪矩阵
	mygun.Update( Device, Camera );
	mygun.Render( Device, Camera );

	if(enemy->GetState() ==2)//如果死掉
	{
		enemy->Die();
		enemydead = true;
		if(!renderstop)
			enemy->Render(Device,time);
	}
	else
		enemy->Render(Device,time);//没有死，继续

	char fps[20],HP[10],Mystrength[10];
	sprintf(fps,"FPS:%f",Timer->GetFPS());
	RECT rect1;
	rect1.left = 0;
	rect1.top = 30;
	rect1.right = 100;
	rect1.bottom = 50;
	font->DrawTextA(NULL, fps, (int)strlen(fps), &rect1, DT_BOTTOM, D3DCOLOR_XRGB(81,181,6));

	Device->EndScene();
	Device->Present( NULL, NULL, NULL, NULL);
}
