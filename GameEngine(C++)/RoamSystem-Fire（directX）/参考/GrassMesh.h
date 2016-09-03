#pragma once
#include "Common.h"

#define GRASSVERTEX_FVF (D3DFVF_XYZ | D3DFVF_TEX1)

struct GRASSVERTEX
{
	GRASSVERTEX(float x, float y, float z,float u, float v)
	{
		this->x = x; this->y = y; this->z = z;
		this->u = u; this->v = v;
	}
	float x,y,z;
	float u,v;
};

class GrassMesh
{
public:
	GrassMesh();
	void InitMesh(IDirect3DDevice9 *Device);
	void DrawMesh(IDirect3DDevice9 *Device, float posx,float posy,float posz);
	~GrassMesh(void);
private:

	IDirect3DTexture9 *GrassMeshtex;
	IDirect3DVertexBuffer9 *VertexBuffer;
};