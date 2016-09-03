//世界里的物品类
#pragma once
#include "Common.h"
#include "Camera.h"
#include "Terrain.h"
#include "SkinMesh.h"

class Item
{
public:
	Item(IDirect3DDevice9 *Device);
	~Item(void);
	void Init(IDirect3DDevice9 *Device);
	void CheckDistance(CCamera *Camera);//根据距离决定状态，0正常，1被吃掉=加血
	void Update(CCamera *Camera,float time);//根据距离更新状态，处理自身旋转
	void Render(IDirect3DDevice9 *Device,float time);//渲染，状态1不用渲染
	int GetState();
	float GetScale();
	bool Overdue();

private:
	CCamera *Camera;
	CSkinMesh *item;
	int state;//状态，0普通，1被吃掉
	bool effect;//吃过了，过期作废。。。
	D3DXVECTOR3 pos;//位置
	float angle;//朝向
	float distance;//离玩家距离
	float walkspeed;
	float Scale;//缩放大小
};
