#pragma once
#include <vector>
#include "Common.h"
#include "Frustum.h"
#include "Input.h"
#include "Timer.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Sound.h"
#include "MyGun.h"
#include "Enemy.h"
#include "GrassMesh.h"
#include "Billboard.h"
#include "Quad.h"
#include "Item.h"

#define GUI_FVF (D3DFVF_XYZRHW | D3DFVF_TEX1)

struct GUIVERTEX
{
	GUIVERTEX(float x, float y, float z, float rhw, float u, float v)
	{
		this->x = x; this->y = y; this->z = z; this->rhw =rhw;
		this->u = u; this->v = v;
	}
	float x,y,z,rhw;
	float u,v;
};

using namespace std;

class FPSGame
{
public:
	FPSGame(IDirect3DDevice9 *Device, HWND hWnd, HINSTANCE hInstance);
	~FPSGame(void);
	void GameLoop(IDirect3DDevice9 *Device);
	bool ray_intersect_AABB(D3DXVECTOR3 pos,D3DXVECTOR3 dir,D3DXVECTOR3 min,D3DXVECTOR3 max,float &hitdistance);
private:
	//字体
	ID3DXFont *font;
	D3DXCOLOR fontColor;

	//输入
	CInput *Input;

	//定时器
	CTimer *Timer;

	//材质
	CMaterial *Material;

	//灯光
	CLight *LightforTerrain,*LightforGun;


	//摄像机
	CCamera *Camera;

	//平截头体
	CFrustum *Frustum;

	//声音系统
	CSoundManager *SoundManager;
    CSound *Sound1;
	CSound *Sound2;
	CSound *Sound3;
	CSound *Sound4;
	CSound *Sound5;

	//天空盒
	SkyBox skybox;

	//敌人
	Enemy *enemy;
	//第一人称枪
	MyGun mygun;
	float time;
	float CycleTimer1;//声音1计时器
	float CycleTimer4;//声音4计时器
	float soundcycle1;//声音周期1,走路
	float soundcycle4;//声音周期4，砍人
	float CycleTimer3;//声音3计时器
	float soundcycle3;//声音周期1,开火
	float waitTimer;//敌人死亡后等待计时
	float velocity; //移动加速度
	bool Sound1ready;
	bool Sound4ready;
	bool Sound3ready;
	//地形
	CTerrain *terrain;

	int myHP;
	float strength;//力气，没了就不能跑了
	float hitdistance;//击中距离。。。
	bool run;//跑动？
	
	D3DXVECTOR3 vMin,vMax;//敌人AABB包围盒

	Item *firstaid[5];//随机血包
	CSkinMesh *bone[8];

	//敌人血条
	CBillboard *BloodBar;
	CBillboard *BloodBarCase;
	bool enemydead;//敌人是否挂了
	bool youdead;//你挂了
	bool renderstop;//敌人是否还渲染

	//画面
	IDirect3DTexture9 *tex_over;
	IDirect3DVertexBuffer9 *guiVertexBuffer;
};