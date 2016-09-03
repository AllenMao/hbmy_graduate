#pragma once
#include "Common.h"
#include <vector>
#include <fstream>
using namespace std;

#define TERRAINVERTEX_FVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2)

struct TERRAINVERTEX
{
	TERRAINVERTEX(float x, float y, float z, float nx, float ny, float nz, float u, float v,float u1,float v1)
	{
		this->x = x; this->y = y; this->z = z;
		this->nx = nx; this->ny = ny; this->nz = nz;
		this->u = u; this->v = v;
		this->u1 =u1,this->v1=v1;
	}
	float x,y,z;
	float nx,ny,nz;
	float u,v;
	float u1,v1;
};

class CTerrain
{
public:
	CTerrain(int r, int c, float length);
	~CTerrain(void);
	bool InitTerrain(IDirect3DDevice9 *Device, LPCSTR rawFileName, LPCSTR texFilename,LPCSTR texFilename1, float heightScale, bool OneTex = true);
	void Render(IDirect3DDevice9 *Device);
	float GetHeight(int r, int l);
	void ComputeTriNormal(TERRAINVERTEX &v1, TERRAINVERTEX &v2, TERRAINVERTEX &v3, D3DVECTOR &normal);
	float GetCameraHeight(float x, float z);
	float Lerp(float a, float b, float t);
private:
	int num_row;								//每列顶点数
	int num_col;								//每行顶点数
	float uTexSize;								//平分后的纹理U坐标
	float vTexSize;								//平分后的纹理V坐标
	IDirect3DVertexBuffer9 *m_pVertexBuffer;
	IDirect3DTexture9 *Dtex;
	IDirect3DTexture9 *Dtex1;
	IDirect3DIndexBuffer9 *m_pIndexBuffer;
	float length;								//网格宽度
	vector<float> raw;							//存放高度的容器
};
