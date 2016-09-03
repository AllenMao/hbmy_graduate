#pragma once

/** 建立处于XZ平面的平面 
*/
class CQuad
{
	LPDIRECT3DDEVICE9		m_pDevice;

	/// 顶点
	LPDIRECT3DVERTEXBUFFER9 m_pMeshVertices;
	/// 纹理
	LPDIRECT3DTEXTURE9 m_pTexture;

	int		m_nNumVertsAlongX;
	int		m_nNumVertsAlongZ;
	float	m_fMeshLengthAlongX;
	float	m_fMeshLengthAlongZ;

	int		m_nType;

public:
	struct QuadVertex
	{
		float x, y, z;
		float nx, ny, nz;
		DWORD color;
		float tu, tv;

		enum FVF
		{
			FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1
		};
	};


	CQuad(void);
	~CQuad(void);

	/** 初始化
		@param
			fSizeX X方向大小
		@param
			fSizeZ Z方向大小
		@param
			nType 初始朝向类型 0-X轴方向,1-Y轴方向,2-Z轴方向
	*/
	void Init( LPDIRECT3DDEVICE9 pDevice, float fSizeX, float fSizeZ, const char* pTexName, int nType = 2 );
	void Render( D3DXMATRIX& matWorld );
	void SetSizeX(float fX);
};
