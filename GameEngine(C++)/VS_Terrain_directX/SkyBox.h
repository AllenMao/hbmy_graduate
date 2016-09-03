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
	SkyBox(){};
	SkyBox( IDirect3DDevice9 *Device );
	~SkyBox(void);
	//������������
	bool InitSky( LPCSTR fontFileName, LPCSTR backFileName, LPCSTR leftFileName, LPCSTR rightFileName, LPCSTR topFileName, LPCSTR bottomFileName, float skywidth, float skyheight);
	//��Ⱦ
	void Render( D3DXMATRIX *pos=NULL);
	//������պ��ӵĸ߶ȺͿ��
	void SetWidHei(float w, float h);
private:
	IDirect3DDevice9 *m_pd3dDevice;
	IDirect3DTexture9 *Skytex[6];
	IDirect3DVertexBuffer9 *m_pVertexBuffer;
	float					m_fHeight;//���������߶�
};
