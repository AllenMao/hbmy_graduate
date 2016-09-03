#include "Light.h"

CLight::CLight(void)
{
}

CLight::~CLight(void)
{
}

void CLight::SetPointLight(IDirect3DDevice9 *device, D3DXVECTOR3 &position, D3DXCOLOR color)
{
	ZeroMemory(&d3dlight,sizeof(d3dlight));
	d3dlight.Type			= D3DLIGHT_POINT;
	d3dlight.Ambient		= color;
	d3dlight.Diffuse		= color * ((rand()%5+13)*0.1f);
	d3dlight.Specular		= color * ((rand()%5+13)*0.1f);
	d3dlight.Position		= position;
	d3dlight.Range			= (rand()%7+13)*10.0f;
	d3dlight.Falloff		= 1.0f;
	d3dlight.Attenuation0	= 1.0f;
	d3dlight.Attenuation1	= 0.0f;
	d3dlight.Attenuation2	= 0.0f;
	device->SetLight(1,&d3dlight);
	
}

void CLight::SetSpotLight(IDirect3DDevice9 *device,D3DXVECTOR3 &position, D3DXVECTOR3 &direction, D3DXCOLOR color)
{
	ZeroMemory(&d3dlight,sizeof(d3dlight));
	d3dlight.Type			= D3DLIGHT_SPOT;
	d3dlight.Ambient		= color * 1.1f;
	d3dlight.Diffuse		= color;
	d3dlight.Specular		= color * 1.5f;
	d3dlight.Position		= position;
	d3dlight.Direction		= direction;
	d3dlight.Range			= 500.0f;
	d3dlight.Falloff		= 1.0f;
	d3dlight.Attenuation0	= 1.0f;
	d3dlight.Attenuation1	= 0.0f;
	d3dlight.Attenuation2	= 0.0f;
	d3dlight.Theta			= 0.7f;
	d3dlight.Phi			= 0.5f;
	device->SetLight(0,&d3dlight);
	device->LightEnable(0,true);
}

void CLight::SetDirectionLight(IDirect3DDevice9 *device, D3DXVECTOR3 &direction, D3DXCOLOR color)
{
	ZeroMemory(&d3dlight,sizeof(d3dlight));
	d3dlight.Diffuse	= color;
	d3dlight.Type		= D3DLIGHT_DIRECTIONAL;
	d3dlight.Direction	= direction;
	device->SetLight(0,&d3dlight);
	device->LightEnable(0,true);
}

void CLight::SetDirectionLightforGun(IDirect3DDevice9 *device, D3DXVECTOR3 &direction, D3DXCOLOR color)
{
	ZeroMemory(&d3dlight,sizeof(d3dlight));
	d3dlight.Ambient		= color * 0.4f;
	d3dlight.Diffuse		= color;
	d3dlight.Specular		= color * 0.6f;
	d3dlight.Type		= D3DLIGHT_DIRECTIONAL;
	d3dlight.Direction	= direction;
	device->SetLight(0,&d3dlight);
	device->LightEnable(0,true);
}