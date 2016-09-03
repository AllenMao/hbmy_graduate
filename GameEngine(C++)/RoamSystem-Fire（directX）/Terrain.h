#pragma once
#include "Common.h"
#include <vector>
#include <fstream>
using namespace std;

class CTerrain
{
private:
	LPDIRECT3DDEVICE9       m_pd3dDevice;           //D3D�豸  
	LPDIRECT3DTEXTURE9      m_pTexture;				//����
	LPDIRECT3DTEXTURE9      m_pTexture1;
    LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;         //���㻺��  
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;        //��������

	int                         m_iCellsPerRow;     // ÿ�еĵ�Ԫ����  
    int                         m_iCellsPerCol;         // ÿ�еĵ�Ԫ����  
    int                         m_iVertsPerRow;     // ÿ�еĶ�����  
    int                         m_iVertsPerCol;         // ÿ�еĶ�����  
    int                         m_iNumVertices;     // ��������  
    FLOAT                       m_fTerrainWidth;        // ���εĿ��  
    FLOAT                       m_fTerrainDepth;        // ���ε����  
    FLOAT                       m_fCellSpacing;         // ��Ԫ��ļ��  
    FLOAT                       m_fHeightScale;         // �߶�����ϵ��
	vector<FLOAT>				m_vHeightInfo;			// ���ڴ�Ÿ߶���Ϣ 
	
	//����һ�����ε�FVF�����ʽ
	struct TERRAINVERTEX
	{
		FLOAT _x, _y, _z;
		FLOAT _nx, _ny, _nz;
        FLOAT _u, _v;
		FLOAT _u1, _v1;
        TERRAINVERTEX(float x, float y, float z, float nx, float ny, float nz, float u, float v,float u1,float v1)   
		{
			_x = x; _y = y; _z = z;
			_nx = nx; _ny = ny; _nz = nz;
			_u = u; _v = v;
			_u1 =u1,_v1=v1;
		}
        static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2;
	};

public:
	CTerrain( ){};
	CTerrain( IDirect3DDevice9 *pd3dDevice );
	virtual ~CTerrain();

public:
    BOOL LoadTerrainFromFile( char* pRawFileName, char* pTextureFile, char* pTextureFile1 );		//���ļ����ظ߶�ͼ������ĺ���
    BOOL InitTerrain( INT nRows, INT nCols, FLOAT fSpace, FLOAT fScale );  //���γ�ʼ������
    void Render(IDirect3DDevice9 *Device);  //������Ⱦ����
	
	float GetCameraHeight(float x, float z);
	float Lerp(float a, float b, float t);
	float GetHeight(int r, int l);
	void ComputeTriNormal(TERRAINVERTEX &v1, TERRAINVERTEX &v2, 
		TERRAINVERTEX &v3, D3DVECTOR &normal);

};
