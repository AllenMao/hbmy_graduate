#pragma once
#include "Common.h"
#define SKYVERTEX_FVF (D3DFVF_XYZ | D3DFVF_TEX1)

struct SKYVERTEX
{
	SKYVERTEX(float x, float y, float z, float u, float v)
	{
		this->x = x; this->y = y; this->z = z;
		this->u = u; this->v = v;
	}
	float x,y,z;
	float u,v;
};

class SkyBox
{
public:
	SkyBox();
	~SkyBox(void);
	//������������
	bool InitSky(IDirect3DDevice9 *Device, LPCSTR fontFileName, LPCSTR backFileName, LPCSTR leftFileName, LPCSTR rightFileName, LPCSTR topFileName, LPCSTR bottomFileName, float skywidth, float skyheight);
	//��Ⱦ
	void Render(IDirect3DDevice9 *Device, D3DXMATRIX *pos=NULL);
	//������պ��ӵĸ߶ȺͿ��
	void SetWidHei(float w, float h);
private:
	IDirect3DTexture9 *Skytex[6];
	IDirect3DVertexBuffer9 *m_pVertexBuffer;
};
