#pragma once
#include "Common.h"
#include "CImage.h"
#include "CFrustum.h"
#include "CCamera.h"
#include <vector>
#include <fstream>
using namespace std;

#define TRN_NUM_TILES 4

enum ETILE_TYPES
{
	LOWEST_TILE= 0,		//sand, dirt, etc.
	LOW_TILE,			//grass
	HIGH_TILE,			//mountainside
	HIGHEST_TILE		//tip of mountain
};

struct STRN_TEXTURE_REGIONS
{
	int m_iLowHeight;			//lowest possible height (0%)
	int m_iOptimalHeight;		//optimal height (100%)
	int m_iHighHeight;			//highest possible height (0%)
};

struct STRN_TEXTURE_TILES
{
	STRN_TEXTURE_REGIONS m_regions[TRN_NUM_TILES];	//texture regions
	CImage textureTiles[TRN_NUM_TILES];				//texture tiles
	int iNumTiles;
};
//////////////////////////////////////////////////////////////////////////
typedef struct NODEPOSSIZE
{
public:
	NODEPOSSIZE() {};
	NODEPOSSIZE( UINT _x, UINT _y, UINT _size ) {x = _x; y = _y; size = _size;}
	UINT x;
	UINT y;
	UINT size;
}NODEPOSSIZE;

typedef struct tagTRNVERTEX
{
	D3DXVECTOR3 position; // The position of the vertex.
	D3DXVECTOR3 normal;   // The nomal Vector.
	
	D3DCOLOR    color;    // The color
	FLOAT       tu, tv;   // The texture coordinates
}TRNVERTEX;

#define D3DFVF_TRNVERTEX  D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1

typedef enum { HEADNODE = 0, TOPLEFT, BOTTOMLEFT, BOTTOMRIGHT, TOPRIGHT } LODNodeEnum;

class CLODTrnNode
{
public:
	CLODTrnNode();
	CLODTrnNode( NODEPOSSIZE nodePosSize );
	~CLODTrnNode();

public:
	// 公有成员
	// 父节点
	CLODTrnNode* m_pParentNode;
	// 子节点
	CLODTrnNode* m_pTLChildNode;
	CLODTrnNode* m_pBLChildNode;
	CLODTrnNode* m_pTRChildNode;
	CLODTrnNode* m_pBRChildNode;
protected:
	NODEPOSSIZE m_nodePosSize;
public:
// 方法
	// 分裂节点
	void SplitNode();
	NODEPOSSIZE GetPosSize() { return m_nodePosSize; }
	
// 属性
public:
	BOOL IsSplit();
	LODNodeEnum m_EnumValue;
	UINT  m_level;  // m_level越低，LOD级别越高，节点细节越好，头节点的m_level最高
	bool m_bIsLeaf; // 表明该节点是否叶子节点，初始化为true
	bool m_bReachAcceptableLevel; // 表明该节点的level是否已足够低，初始化为false
	bool m_bTouched; // 遍历四叉树的时候是否访问过该节点，初始化为false
	bool m_bIsVisible; // 表明该节点是否可见，初始化为false
};


class CTerrain
{
private:
	unsigned char *m_ucpHeightData;
	int m_iSize;
	//texture information
	STRN_TEXTURE_TILES m_tiles;
	CImage m_texture;
	bool   m_bTextureMapping;
	
public:
	CTerrain(){};
	CTerrain(IDirect3DDevice9* pd3dDevice);
	~CTerrain();
	bool SaveHeightMap( char* szFilename );

	//fractal terrain generation
	void NormalizeTerrain( float* fpHeightData );
	void FilterHeightBand( float* fpBand, int iStride, int iCount, float fFilter );
	void FilterHeightField( float* fpHeightData, float fFilter );
	
	bool MakeTerrainFault( int iSize, int iIterations, int iMinDelta, int iMaxDelta, float fFilter );
	
	//texture map generation functions
	float RegionPercent( int tileType, unsigned char ucHeight );
	void GetTexCoords( CImage texture, unsigned int* x, unsigned int* y );
	unsigned char InterpolateHeight( int x, int z, float fHeightToTexRatio );

	//texture map generation
	void GenerateTextureMap( unsigned int uiSize );

	//--------------------------------------------------------------
	// Name:			CTERRAIN::GetTrueHeightAtPoint - public
	// Description:		A function to get the true height value (0-255) at a point
	// Arguments:		-iX, iZ: which height value to retrieve
	// Return Value:	An float value: the true height at
	//					the given point
	//--------------------------------------------------------------
	inline unsigned char GetTrueHeightAtPoint( int x, int z )
	{	return ( m_ucpHeightData[( z*m_iSize )+x] );	}


private:
	LPDIRECT3DDEVICE9       m_pd3dDevice;           //D3D设备  
	LPDIRECT3DTEXTURE9      m_pTexture;				//纹理
	LPDIRECT3DTEXTURE9      m_pTexture1;
	
public:
    BOOL LoadTerrainFromFile( char* pRawFileName, char* pTextureFile, char* pTextureFile1 );		//从文件加载高度图和纹理的函数
	/////////////////////LOD//////////////////////////////////////
	// 公有方法
	HRESULT InitShader();
	void InitQuadTree();
	void SetDimesion( float width, float height/*地形的几何尺寸*/, D3DXVECTOR2 centerPt/*地形的中点位置*/ );
	void CreateTerrain(CCamera* pCamera );
	void RenderTerrain(BOOL bFog );
	// 给定X坐标和Y坐标，求出该点的地形高度
	float GetTerrainElev( float x, float z );

	float GetSizeY() { return m_fHeight; }
	float GetSizeX() { return m_fWidth; }
	float GetPosX(){ return m_centerPt.x; }
	float GetPosY(){ return m_centerPt.y; }

protected:
	// 将头结点生长成一棵完全的二叉树
	void GrowQuadTree(CLODTrnNode* pNode);
	// 将该节点及其所以子节点的属性（m_bReachAcceptableLevel，m_bTouched，m_bIsVisible）修改为false
	void ClearTreeFlag(CLODTrnNode* pNode);
	// 遍历四叉树，判断节点的可见性和它的level是否达到要求，并对节点进行标记
	void ClimbQuadTree(CLODTrnNode* pNode, LPDIRECT3DDEVICE9 pd3dDevice);
	// 判断某个节点是否可见
	BOOL IsVisible(CLODTrnNode* pNode, LPDIRECT3DDEVICE9 pd3dDevice);
	// 判断某个节点的level是否已足够低
	BOOL IsReachAcceptableLevel(CLODTrnNode* pNode);
	// 得到节点的四个相邻节点的父节点，将它们的指针存储在一个数组里
	void GetAdjNodeParent4(CLODTrnNode** pAdjNodeArray, CLODTrnNode* pNode);
	// 查找相对位置为(x, y)的那个节点的父节点
	CLODTrnNode* FindParentNode(UINT x, UINT y);
	// 查找相对位置为(x, y)的那个节点
	CLODTrnNode* FindNode(UINT x, UINT y);
	// 遍历四叉树，寻找树中m_bReachAcceptableLevel == true的节点，并将所有符合条件的节点写进顶点数组和索引数组中
	void RefineNode(CLODTrnNode* pNode);
protected:
	/*UINT m_height;*/
	UINT m_width;
	CLODTrnNode* m_pHeadNode;

	// 地形的几何尺寸及中心位置
	float m_fWidth;
	float m_fHeight;

	float m_GridHeight; // 网格宽度，加快计算速度
	float m_GridWidth;  // 网格高度，加快计算速度
	D3DXVECTOR2 m_centerPt;
	// 可视裁减体
	CFrustum m_frustum;
	// 摄像机
	CCamera* m_pCamera;

	// 二维高程数组
	float** m_elevArray;
	// 二维法线向量数组
	D3DXVECTOR3 ** m_normalArray;
	// 纹理坐标数组
	float* m_tuArray;
	float* m_tvArray;
	// 需要渲染得节点数
	UINT m_numNodeRendered;
	// 顶点数组
	TRNVERTEX *m_vertexArray;
	// 索引数组
	WORD *m_indexArray;
	// 顶点索引
	UINT m_vertexIndex;
	// 索引缓冲索引
	UINT m_indexIndex;
	// 顶点缓冲
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	// 索引缓冲
    LPDIRECT3DINDEXBUFFER9 m_pIB;

	// Shader
	IDirect3DVertexShader9* m_pVShader;
	IDirect3DPixelShader9 * m_pPShader;
	ID3DXConstantTable* m_pVConstTable;
	ID3DXConstantTable* m_pPConstTable;
	// Constant table
};

// 将该节点及其所以子节点的属性（m_bReachAcceptableLevel，m_bTouched，m_bIsVisible）修改为false
inline void CTerrain::ClearTreeFlag(CLODTrnNode* pNode)
{
	
	//pNode->m_bReachAcceptableLevel = false;
	//pNode->m_bIsVisible = false;
	pNode->m_bTouched = false;
	if(!pNode->m_pBLChildNode->m_bIsLeaf)
	{
		ClearTreeFlag( pNode->m_pBLChildNode );
		ClearTreeFlag( pNode->m_pTLChildNode );
		ClearTreeFlag( pNode->m_pTRChildNode );
		ClearTreeFlag( pNode->m_pBRChildNode );
	}
	{
		pNode->m_pBLChildNode->m_bTouched = false;
		pNode->m_pTLChildNode->m_bTouched = false;
		pNode->m_pTRChildNode->m_bTouched = false;
		pNode->m_pBRChildNode->m_bTouched = false;
	}
}