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
	LPDIRECT3DVERTEXBUFFER9     m_pTreeVB; // ��ľ���㻺����
	LPDIRECT3DTEXTURE9          m_pTreeTex; // ��ľ����

	D3DXVECTOR3					m_vCenterPos;           // ����λ��
	int							m_iWidth;
	int							m_iHeight;
public:
	CBillboard(void){};
	CBillboard(IDirect3DDevice9 *pd3dDevice, D3DXVECTOR3 vCenterPos, int iWidth = 40, int iHeight = 60);
	~CBillboard(void);

	void FlushVertexBuffer(BILLBOARDVERTEX* pVertices, int& index);     // ���л����㻺��
	void FlushIndexBuffer(DWORD* pIndices, int& index);                   // ���л���������

	void Init(char *szFileName);
	void Render(void);
};

float GetRandomFloat(float min, float max);                               // ��ȡһ��float�����


