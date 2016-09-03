#pragma once
#include "Common.h"

struct BILLBOARDVERTEX
{
	D3DXVECTOR3 vVPos;
	D3DXVECTOR2 vUV;
	BILLBOARDVERTEX(D3DXVECTOR3 Pos, D3DXVECTOR2 UV)  
		: vVPos(Pos), vUV(UV){}   
};
#define D3DFVF_BILLBOARDVERTEX (D3DFVF_XYZ |D3DFVF_TEX1)

class CBillboard
{
private:
	LPDIRECT3DDEVICE9           m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9     m_pTreeVB; // 树木顶点缓冲区
	LPDIRECT3DTEXTURE9          m_pTreeTex; // 树木纹理

	D3DXVECTOR3					m_vCenterPos;           // 中心位置
	int							m_iWidth;
	int							m_iHeight;
public:
	CBillboard(void){};
	CBillboard(IDirect3DDevice9 *pd3dDevice, D3DXVECTOR3 vCenterPos, int iWidth = 40, int iHeight = 60);
	~CBillboard(void);

	void FlushVertexBuffer(BILLBOARDVERTEX* pVertices, int& index);     // 序列化顶点缓冲
	void FlushIndexBuffer(DWORD* pIndices, int& index);                   // 序列化索引缓冲

	void Init(char *szFileName);
	void Render(void);
};

float GetRandomFloat(float min, float max);                               // 获取一个float随机数


