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
	GUIVERTEX *vertex;
	D3DXCreateTextureFromFile(Device,"res/texture/gameover.bmp",&tex_over);
	Device->CreateVertexBuffer(4 * sizeof(GUIVERTEX), 0, GUI_FVF, D3DPOOL_MANAGED, &guiVertexBuffer, 0);

	guiVertexBuffer->Lock(0, 0, (void**)&vertex, 0);

	vertex[0] = GUIVERTEX(0.0f, (float)-rect.bottom, 0.0f,1.0f, 0.0f, 1.0f);
	vertex[1] = GUIVERTEX(0.0f, 0.0f, 0.0f, 1.0f,0.0f, 0.0f);
	vertex[2] = GUIVERTEX((float)rect.right, (float)-rect.bottom, 0.0f,1.0f, 1.0f, 1.0f);
	vertex[3] = GUIVERTEX((float)rect.right, 0.0f, 0.0f, 1.0f,1.0f, 0.0f);
	guiVertexBuffer->Unlock();


	myHP = 20;//自己的血
	float strength =2.0;
	Frustum = new CFrustum;
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
	Timer = new CTimer;
	Material = new CMaterial;
	LightforTerrain = new CLight;
	LightforGun =new CLight;
	Camera = new CCamera;

	Timer->InitTimer();
	time = Timer->GetPlayTime();
	CycleTimer1 = 0;
	CycleTimer4 = 0;
	soundcycle4 = 0.2f;
	CycleTimer3 =0;
	soundcycle3 = 0.04f;
	waitTimer =0.0f;
	terrain = new CTerrain(256, 256, 40.0f);
	terrain->InitTerrain(Device, "res/texture/heightmap.raw", "res/texture/ground.jpg","res/texture/detail.jpg", 0.8f, false);


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
	skybox.InitSky(Device, "res/texture/Front1.jpg", "res/texture/Back1.jpg", "res/texture/Left1.jpg",
		"res/texture/Right1.jpg", "res/texture/Up1.jpg", "res/texture/Down1.jpg", 8000.0f, 6500.0f);

	//初始枪
	mygun.Init(Device);

	enemy = new Enemy(Device);
	enemy->Init(Device,terrain);

	//物品
	for(int i=0; i<5;i++)
	{
		firstaid[i] = new Item(Device);
		firstaid[i]->Init(Device);
	}

	//场景的骨头
	for(int i=0;i<8;i++)
	{
		bone[i] = new CSkinMesh(Device);
		bone[i]->LoadFromXFile("res/model/bone.x");
	}
	bone[0]->SetPosition(3000,43,-2100);
	bone[0]->SetDirection(rand()%1-1);
	bone[0]->SetScale(3.0f);
	bone[1]->SetPosition(300,10,430);
	bone[1]->SetDirection(rand()%1-1);
	bone[1]->SetScale(2.9f);
	bone[2]->SetPosition(-3000,10,3300);
	bone[2]->SetDirection(rand()%1-1);
	bone[2]->SetScale(3.8f);
	bone[3]->SetPosition(-200,22,-1100);
	bone[3]->SetDirection(rand()%1-1);
	bone[3]->SetScale(3.2f);
	bone[4]->SetPosition(900,32,-430);
	bone[4]->SetDirection(rand()%1-1);
	bone[4]->SetScale(2.9f);
	bone[5]->SetPosition(-1800,50,300);
	bone[5]->SetDirection(rand()%1-1);
	bone[5]->SetScale(3.8f);
	bone[6]->SetPosition(3600,16,-100);
	bone[6]->SetDirection(rand()%1-1);
	bone[6]->SetScale(3.0f);
	bone[7]->SetPosition(700,10,-4300);
	bone[7]->SetDirection(rand()%1-1);
	bone[7]->SetScale(2.9f);
	//场景仙人掌1

	//血条，血槽
	BloodBarCase = new CBillboard();
	BloodBarCase->Init(Device, D3DXVECTOR2(50.0f, 3.0f), "res/texture/case.jpg", BT_CAMERA);
	BloodBarCase->SetPosition(D3DXVECTOR3(500.0f, 350.0f, 500.0f));

	BloodBar = new CBillboard();
	BloodBar->Init(Device, D3DXVECTOR2(48.0f, 2.0f), "res/texture/bar.jpg", BT_CAMERA);
	BloodBar->SetPosition(D3DXVECTOR3(500.0f, 350.0f, 500.0f));
	enemydead =false;
	youdead =false;
}

FPSGame::~FPSGame(void)
{
}

void FPSGame::GameLoop(IDirect3DDevice9 *Device)
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

	if(CycleTimer3 >soundcycle3)
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
		if( waitTimer > 0.8f)
		{
			renderstop = true;
		}
		else
		{
			waitTimer += time;
			renderstop = false;
		}
	}

	this->Timer->UpdateFPS();
	//读取鼠标，控制摄像机
	this->Input->Update();

	Camera->Turn(Input->GetDeltaX() * time * 1.5f);

	//限制摄像机角度.....???
	Camera->Pitch(Input->GetDeltaY() * time* 1.5f);
	////////////////开火///////////////////
	if(Input->GetButtonPress(0,true))
	{
		//加入抖动，火舌等效果
		mygun.Fire();
		if(Sound3ready)
		{
			Sound3->Play(false);
			D3DXVECTOR3 dir;
			Camera->GetLook(&dir);
			if(ray_intersect_AABB(mygun.muzzlepos,dir,enemy->GetMin(),enemy->GetMax(),hitdistance))
				enemy->Hurt();
		}
		Device->LightEnable(1,true);//开灯
	}
	else{
		mygun.Fired();
		Device->LightEnable(1,false);//关灯

	}
	///////////////加子弹//////////////////
	if(Input->GetKeyPress(DIK_R,false))
	{
		//实现困难。。。。
	}

	//读取键盘，控制
	if(Input->GetKeyPress(DIK_ESCAPE,false))
	{
		PostQuitMessage(0);
	}

	if( Input->GetKeyPress(DIK_W,true)){
		Camera->Walk( 230.0f * velocity*time);
		if(Sound1ready)
			Sound1->Play(false);
	}
	if( Input->GetKeyPress(DIK_S,true)){
		Camera->Walk(-160.0f * velocity*time);
		if(Sound1ready)
			Sound1->Play(false);

	}
	if( Input->GetKeyPress(DIK_A,true)){
		Camera->Strafe(-190.0f * velocity*time);
		if(Sound1ready)
			Sound1->Play(false);

	}
	if( Input->GetKeyPress(DIK_D,true)){
		Camera->Strafe( 190.0f * velocity*time);
		if(Sound1ready)
			Sound1->Play(false);

	}
	if( Input->GetKeyPress(DIK_LSHIFT,true)){
		velocity =1.0 + strength;
		soundcycle1 = 0.28f -strength*0.004f;
		run = true;
	}
	else {velocity =0.8f;soundcycle1 = 0.28f;run =false;}

	if(run){
		strength -=time*0.5;
		if(strength < 0)
			strength =0;
	}
	if(!run){
		strength +=time*1.5;
		if(strength>2.0)
			strength =2.0;
	}

	if( Input->GetKeyPress(DIK_E,true))
		Camera->Roll(-1.0f *time);

	if( Input->GetKeyPress(DIK_Q,true))
		Camera->Roll(1.0f * time);

	//设置摄像机高度和天空位置
	D3DXMATRIX skyboxMatrix;
	D3DXVECTOR3 camerapos;
	Camera->GetPos(&camerapos);
	//限制移动，不要超出terrain
	if(camerapos.x > 4000 ) camerapos.x =4000;
	if(camerapos.x <-4000 ) camerapos.x =-4000;
	if(camerapos.z > 4000 ) camerapos.z =4000;
	if(camerapos.z <-4000 ) camerapos.z =-4000;

	D3DXMatrixTranslation(&skyboxMatrix, camerapos.x, -3200.0f, camerapos.z);
	float terrainheight = terrain->GetCameraHeight(camerapos.x,camerapos.z);
	camerapos.y = terrainheight + 80.0f;
	Camera->SetPos(*(&camerapos));

	//摆放摄像机
	D3DXMATRIX V = this->Camera->GetMatrix();
	Device->SetTransform(D3DTS_VIEW, &V);
	Camera->SetCamera(Device, (float)rect.right, (float)rect.bottom);



	BloodBar->Update(Camera);//对着摄像机
	BloodBarCase->Update(Camera);//对着摄像机


	/////////////////////////////更新枪矩阵///////////////////////////////////////////////
	//更新枪矩阵
	mygun.Update(Device,Camera,time);

	//更新敌人
	if(enemy->GetHP()>0)
	{
		enemy->Move(Camera,terrain);
	}
	else
	{
		enemy->Die();
	}
	//画敌人血条
	BloodBar->SetSizeX(enemy->GetHP());
	D3DXVECTOR3 enemypos = enemy->GetPos();
	enemypos.y +=110;
	BloodBar->SetPosition(enemypos);
	BloodBarCase->SetPosition(enemypos);


	//更新血
	if(enemy->GetState() ==1 && enemy->GetDistance()<100)
	{

		if(myHP<0){
			youdead =true;//你挂了。。。
		}
		if(Sound4ready){
			Sound4->Play(false);
			myHP -=1;
		}

	}

	//更新血包
	for(int i =0;i<5;i++)
	{
		firstaid[i]->Update(Camera,time);
		if(firstaid[i]->GetState()==1 && firstaid[i]->Overdue()== true)
		{
			//碰撞，并且没被吃过。。。加血，根据血包的大小来加
			myHP +=firstaid[i]->GetScale()*10;
			strength =2.0;
			if(myHP>20)
				myHP =20;

		}
	}

	////////////////////////////////////////////////////////////////////////开始渲染////////////////////////////////////////////////////////////////////////////////////
	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0L);
	Device->BeginScene();
	//设置灯光
	LightforTerrain->SetDirectionLight(Device, D3DXVECTOR3(-0.8f, -1.2f, -1.0f), D3DCOLOR_XRGB(255,255,255));

	D3DXVECTOR3 firelightpos = mygun.muzzlepos;
	firelightpos.z += 8.0f;
	LightforGun->SetPointLight(Device,firelightpos,D3DCOLOR_XRGB(230,120,20));//枪火的光照

	//设置材质
	Material->SetAmbientColor(201,100,20,0);
	Material->SetDiffuseColor(150,90,10,0);
	Material->SetEmissiveColor(15,15,15,50);
	Material->SetPower(0.1);
	Device->SetMaterial(Material->GetMaterial());

	skybox.Render(Device, &skyboxMatrix);
	terrain->Render(Device);

	if(enemy->GetState() ==2)//如果死掉
	{
		enemy->Die();
		enemydead = true;
		if(!renderstop)
			enemy->Render(Device,time);
	}
	else
		enemy->Render(Device,time);//没有死，继续

	mygun.Render(Device,Camera);

	//画血条
	BloodBarCase->Render();
	BloodBar->Render();

	//血包
	for(int i =0;i<5;i++)
		firstaid[i]->Render(Device,time);

	//骨头
	for(int i=0;i<8;i++)
		bone[i]->Render(time);


	char fps[20],HP[10],Mystrength[10];
	sprintf(fps,"FPS:%f",Timer->GetFPS());
	sprintf(HP, "生命值: %d", myHP);
	sprintf(Mystrength, "体力值: %.1f", strength);

	RECT rect1,rect2,rect3,rect4;
	rect1.left = 0;
	rect1.top = 30;
	rect1.right = 100;
	rect1.bottom = 50;

	rect2.top = 500;
	rect2.left = 400;
	rect2.bottom = 580;
	rect2.right = 520;

	rect4.top =500;
	rect4.left =600;
	rect4.right = 800;
	rect4.bottom = 580;

	rect3.top = 0;
	rect3.left = 20;
	rect3.bottom = 50;
	rect3.right = 1000;
	font->DrawTextA(NULL, Mystrength, (int)strlen(Mystrength), &rect4, DT_BOTTOM, D3DCOLOR_XRGB(81,181,6));
	font->DrawTextA(NULL, HP, (int)strlen(HP), &rect2, DT_BOTTOM, D3DCOLOR_XRGB(81,181,6));

	font->DrawTextA(NULL, fps, (int)strlen(fps), &rect1, DT_BOTTOM, D3DCOLOR_XRGB(81,181,6));
	font->DrawTextA(NULL, "说明：WSAD移动，按住SHIFT能跑，但消耗体力，松开SHIFT可快速恢复，血包能恢复体力和生命", (int)strlen("说明：WSAD移动，按住SHIFT能跑，但要消耗体力，松开SHIFT可回复体力，血包能恢复体力和生命"), &rect3, DT_TOP, fontColor);

	Device->EndScene();
	Device->Present( NULL, NULL, NULL, NULL);

}
