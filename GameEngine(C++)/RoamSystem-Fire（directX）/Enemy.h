#pragma once
#include "Common.h"
#include "Camera.h"
#include "Terrain.h"
#include "SkinMesh.h"

class Enemy
{
public:
	Enemy(IDirect3DDevice9 *Device);
	~Enemy(void);
	void Init(IDirect3DDevice9 *Device,CTerrain *Terrain);
	void Attack();
	void Run();
	void Idle();
	void Hurt();
	void Die();
	int GetHP();
	void SetHP(int hp);
	int GetState();
	int GetDistance();
	void Move(CCamera *Camera,CTerrain *Terrain);
	void Render(IDirect3DDevice9 *Device,float time);
	void Stop();
	D3DXVECTOR3 GetPos();
	D3DXVECTOR3 GetMin();
	D3DXVECTOR3 GetMax();
	D3DXVECTOR3 max;
	D3DXVECTOR3 min;
private:
	CCamera *Camera;
	CTerrain *terrain;
	CSkinMesh *enemy;
	int state;//状态，0普通，1攻击，2死亡
	int HP;//血
	D3DXVECTOR3 m_Max;
    D3DXVECTOR3 m_Min;
	D3DXVECTOR3 pos;//位置
	float angle;//朝向
	float distance;//离玩家距离
	float walkspeed;
	float Scale;//缩放大小
	D3DXVECTOR3 vMin,vMax;//包围盒
	//中间变量
	D3DXMATRIXA16 matTran,matRotation;
};
