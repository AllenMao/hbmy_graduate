#pragma once
#include "Common.h"
class CLight
{
public:
	CLight(void);
	~CLight(void);
	void SetPointLight(IDirect3DDevice9 *device, D3DXVECTOR3 &position, D3DXCOLOR color);//设置点光源
	void SetSpotLight(IDirect3DDevice9 *device,D3DXVECTOR3 &position, D3DXVECTOR3 &direction, D3DXCOLOR color);//设置聚光灯
	void SetDirectionLight(IDirect3DDevice9 *device, D3DXVECTOR3 &direction, D3DXCOLOR color);					//设置方向光源
	void SetDirectionLightforGun(IDirect3DDevice9 *device, D3DXVECTOR3 &direction, D3DXCOLOR color);					//设置方向光源
private:
	D3DLIGHT9 d3dlight;
};