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
	//����
	ID3DXFont *font;
	D3DXCOLOR fontColor;

	//����
	CInput *Input;

	//��ʱ��
	CTimer *Timer;

	//����
	CMaterial *Material;

	//�ƹ�
	CLight *LightforTerrain,*LightforGun;


	//�����
	CCamera *Camera;

	//ƽ��ͷ��
	CFrustum *Frustum;

	//����ϵͳ
	CSoundManager *SoundManager;
    CSound *Sound1;
	CSound *Sound2;
	CSound *Sound3;
	CSound *Sound4;
	CSound *Sound5;

	//��պ�
	SkyBox skybox;

	//����
	Enemy *enemy;
	//��һ�˳�ǹ
	MyGun mygun;
	float time;
	float CycleTimer1;//����1��ʱ��
	float CycleTimer4;//����4��ʱ��
	float soundcycle1;//��������1,��·
	float soundcycle4;//��������4������
	float CycleTimer3;//����3��ʱ��
	float soundcycle3;//��������1,����
	float waitTimer;//����������ȴ���ʱ
	float velocity; //�ƶ����ٶ�
	bool Sound1ready;
	bool Sound4ready;
	bool Sound3ready;
	//����
	CTerrain *terrain;

	int myHP;
	float strength;//������û�˾Ͳ�������
	float hitdistance;//���о��롣����
	bool run;//�ܶ���
	
	D3DXVECTOR3 vMin,vMax;//����AABB��Χ��

	Item *firstaid[5];//���Ѫ��
	CSkinMesh *bone[8];

	//����Ѫ��
	CBillboard *BloodBar;
	CBillboard *BloodBarCase;
	bool enemydead;//�����Ƿ����
	bool youdead;//�����
	bool renderstop;//�����Ƿ���Ⱦ

	//����
	IDirect3DTexture9 *tex_over;
	IDirect3DVertexBuffer9 *guiVertexBuffer;
};