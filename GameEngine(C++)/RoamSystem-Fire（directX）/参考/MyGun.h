//武器类，实现第一人称下的武器显示
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
	ID3DXBuffer *pAdjacencyBuffer;		//三角形邻接面信息缓冲区
	ID3DXBuffer *pMaterialsBuffer;		//材质和纹理数据缓冲区
	DWORD numMaterials;					//材质数量
	ID3DXMesh *xmesh;				    //mesh指针
	D3DMATERIAL9 *pMaterialsArray;		//材质数组
	IDirect3DTexture9 **pTextureArray;	//纹理数组
	D3DXMATERIAL *pD3DMaterial;			//扩展后的材质数组,包括纹理名称

	IDirect3DTexture9 *muzzle_tex;
	IDirect3DVertexBuffer9 *muzzle_VB;
	float MuzzleScale;//随机大小
	float MuzzleRot;//随机旋转
	D3DXMATRIX matEmitterWorld;
	CQuad *muzzle;
	bool firestate;//状态：0闲置，1开火
	CLight *light;
};
