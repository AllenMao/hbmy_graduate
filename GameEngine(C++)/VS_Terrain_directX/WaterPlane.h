#pragma once
#include "Common.h"
#include "Timer.h"

typedef struct 
{
	D3DXVECTOR3 position; // The position
	D3DXVECTOR3 normal;   // normal vector
	D3DCOLOR    color;    // The color
	FLOAT       tu1, tv1;   // The texture coordinates
	FLOAT       tu2, tv2;   // The texture coordinates
} VERTEX_XYZ_NORMAL_DIFFUSE_TEX2;

// Our custom FVF, which describes the VERTEX_XYZ_NORMAL_DIFFUSE_TEX1 vertex type
#define D3DFVF_XYZ_NORMAL_DIFFUSE_TEX2 (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2)

class CWaterPlane
{
public:
	CWaterPlane(IDirect3DDevice9* Device);
	CWaterPlane(){};
	virtual ~CWaterPlane();

	virtual HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE9 pDev,
		int iNumVerts, const char *strInPoolTextureFilename);

	virtual void InvalidateDeviceObjects();

	virtual void Update(float fElapsedTime);
	virtual void Render();

	void CreateWaterDroplet(int iX, int iY, int iSize, int iSplashStrength);

	bool GetHardcodeNormals(void) const { return(m_bHardcodeNormals); }
	void SetHardcodeNormals(const bool data = true) { m_bHardcodeNormals = data; }

	float GetEnvBlendFactor(void) const { return(m_fEnvBlendFactor); }
	void SetEnvBlendFactor(const float data) { m_fEnvBlendFactor = data; }

	float GetDepth(void) const { return(m_fDepth); }
	void SetDepth(const float data) { m_fDepth = data; }

	void AnimatingTexture(char* szFileName);

protected:
	void ProcessWater();
	void ApplyHeightArrayToVB();
	float CalcDisplacement(float fHeightdiff);

	HRESULT CreateVertexGrid(D3DPOOL pool, float fTotalSize, DWORD dwColor, 
		int iNumVerticesX, int iNumVerticesY);

	LPDIRECT3DVERTEXBUFFER9 m_pVBPool;
	LPDIRECT3DINDEXBUFFER9 m_pIBPool;
	LPDIRECT3DTEXTURE9 m_texInsidePool;

	LPDIRECT3DDEVICE9 m_pd3dDevice;
	int m_iNumVerts;

	float *m_fHeightArray1;
	float *m_fHeightArray2;

	float *m_pActiveHeightArray;
	float *m_pScratchHeightArray;

	float m_fDampValue;
	float m_fRefractionIndex;
	float m_fDepth;
	float m_fEnvBlendFactor;

	bool m_bHardcodeNormals;

	CTimer m_Timer;

};