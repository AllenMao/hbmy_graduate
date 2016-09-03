#pragma once
#include "Common.h"
class CLight
{
public:
	CLight(void);
	~CLight(void);
	void SetPointLight(IDirect3DDevice9 *device, D3DXVECTOR3 &position, D3DXCOLOR color);//���õ��Դ
	void SetSpotLight(IDirect3DDevice9 *device,D3DXVECTOR3 &position, D3DXVECTOR3 &direction, D3DXCOLOR color);//���þ۹��
	void SetDirectionLight(IDirect3DDevice9 *device, D3DXVECTOR3 &direction, D3DXCOLOR color);					//���÷����Դ
	void SetDirectionLightforGun(IDirect3DDevice9 *device, D3DXVECTOR3 &direction, D3DXCOLOR color);					//���÷����Դ
private:
	D3DLIGHT9 d3dlight;
};