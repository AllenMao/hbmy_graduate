//�����࣬ʵ�ֵ�һ�˳��µ�������ʾ
#pragma once
#include "Common.h"
#include "Camera.h"
#include "Light.h"
#include "Quad.h"
#include "Billboard.h"

#define MUZZLEVERTEX_FVF (D3DFVF_XYZ|D3DFVF_TEX1)

typedef struct 
{
  D3DXVECTOR3 position;
  FLOAT       tu, tv;
} MUZZLEVERTEX;


class MyGun
{
public:
	MyGun();
	~MyGun(void);
	void Init(IDirect3DDevice9 *Device);
	void Update(IDirect3DDevice9 *Device,CCamera *Camera,float time);
	void Render(IDirect3DDevice9 *Device,CCamera *Camera);
	void Fire();
	void Fired();
	D3DXMATRIX GetGunMat(){return matResult;};
	bool GetFireState(){return firestate;};
	D3DXMATRIX matResult;
	D3DXVECTOR3 muzzlepos;
private:
	CCamera *Camera;
	ID3DXBuffer *pAdjacencyBuffer;		//�������ڽ�����Ϣ������
	ID3DXBuffer *pMaterialsBuffer;		//���ʺ��������ݻ�����
	DWORD numMaterials;					//��������
	ID3DXMesh *xmesh;				    //meshָ��
	D3DMATERIAL9 *pMaterialsArray;		//��������
	IDirect3DTexture9 **pTextureArray;	//��������
	D3DXMATERIAL *pD3DMaterial;			//��չ��Ĳ�������,������������

	IDirect3DTexture9 *muzzle_tex;
	IDirect3DVertexBuffer9 *muzzle_VB;
	float MuzzleScale;//�����С
	float MuzzleRot;//�����ת
	D3DXMATRIX matEmitterWorld;
	CQuad *muzzle;
	bool firestate;//״̬��0���ã�1����
	CLight *light;
};
