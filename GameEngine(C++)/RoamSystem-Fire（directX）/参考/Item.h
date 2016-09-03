//���������Ʒ��
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
	void CheckDistance(CCamera *Camera);//���ݾ������״̬��0������1���Ե�=��Ѫ
	void Update(CCamera *Camera,float time);//���ݾ������״̬������������ת
	void Render(IDirect3DDevice9 *Device,float time);//��Ⱦ��״̬1������Ⱦ
	int GetState();
	float GetScale();
	bool Overdue();

private:
	CCamera *Camera;
	CSkinMesh *item;
	int state;//״̬��0��ͨ��1���Ե�
	bool effect;//�Թ��ˣ��������ϡ�����
	D3DXVECTOR3 pos;//λ��
	float angle;//����
	float distance;//����Ҿ���
	float walkspeed;
	float Scale;//���Ŵ�С
};
