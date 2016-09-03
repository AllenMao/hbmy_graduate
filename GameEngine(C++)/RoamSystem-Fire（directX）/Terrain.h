#pragma once
#include "Common.h"
#include <vector>
#include <fstream>
using namespace std;

class CTerrain
{
private:
	LPDIRECT3DDEVICE9       m_pd3dDevice;           //D3D设备  
	LPDIRECT3DTEXTURE9      m_pTexture;				//纹理
	LPDIRECT3DTEXTURE9      m_pTexture1;
    LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;         //顶点缓存  
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;        //索引缓存

	int                         m_iCellsPerRow;     // 每行的单元格数  
    int                         m_iCellsPerCol;         // 每列的单元格数  
    int                         m_iVertsPerRow;     // 每行的顶点数  
    int                         m_iVertsPerCol;         // 每列的顶点数  
    int                         m_iNumVertices;     // 顶点总数  
    FLOAT                       m_fTerrainWidth;        // 地形的宽度  
    FLOAT                       m_fTerrainDepth;        // 地形的深度  
    FLOAT                       m_fCellSpacing;         // 单元格的间距  
    FLOAT                       m_fHeightScale;         // 高度缩放系数
	vector<FLOAT>				m_vHeightInfo;			// 用于存放高度信息 
	
	//定义一个地形的FVF顶点格式
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
    BOOL LoadTerrainFromFile( char* pRawFileName, char* pTextureFile, char* pTextureFile1 );		//从文件加载高度图和纹理的函数
    BOOL InitTerrain( INT nRows, INT nCols, FLOAT fSpace, FLOAT fScale );  //地形初始化函数
    void Render(IDirect3DDevice9 *Device);  //地形渲染函数
	
	float GetCameraHeight(float x, float z);
	float Lerp(float a, float b, float t);
	float GetHeight(int r, int l);
	void ComputeTriNormal(TERRAINVERTEX &v1, TERRAINVERTEX &v2, 
		TERRAINVERTEX &v3, D3DVECTOR &normal);

};
