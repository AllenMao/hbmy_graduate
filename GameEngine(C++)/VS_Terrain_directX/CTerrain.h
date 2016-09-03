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
	// ���г�Ա
	// ���ڵ�
	CLODTrnNode* m_pParentNode;
	// �ӽڵ�
	CLODTrnNode* m_pTLChildNode;
	CLODTrnNode* m_pBLChildNode;
	CLODTrnNode* m_pTRChildNode;
	CLODTrnNode* m_pBRChildNode;
protected:
	NODEPOSSIZE m_nodePosSize;
public:
// ����
	// ���ѽڵ�
	void SplitNode();
	NODEPOSSIZE GetPosSize() { return m_nodePosSize; }
	
// ����
public:
	BOOL IsSplit();
	LODNodeEnum m_EnumValue;
	UINT  m_level;  // m_levelԽ�ͣ�LOD����Խ�ߣ��ڵ�ϸ��Խ�ã�ͷ�ڵ��m_level���
	bool m_bIsLeaf; // �����ýڵ��Ƿ�Ҷ�ӽڵ㣬��ʼ��Ϊtrue
	bool m_bReachAcceptableLevel; // �����ýڵ��level�Ƿ����㹻�ͣ���ʼ��Ϊfalse
	bool m_bTouched; // �����Ĳ�����ʱ���Ƿ���ʹ��ýڵ㣬��ʼ��Ϊfalse
	bool m_bIsVisible; // �����ýڵ��Ƿ�ɼ�����ʼ��Ϊfalse
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
	LPDIRECT3DDEVICE9       m_pd3dDevice;           //D3D�豸  
	LPDIRECT3DTEXTURE9      m_pTexture;				//����
	LPDIRECT3DTEXTURE9      m_pTexture1;
	
public:
    BOOL LoadTerrainFromFile( char* pRawFileName, char* pTextureFile, char* pTextureFile1 );		//���ļ����ظ߶�ͼ������ĺ���
	/////////////////////LOD//////////////////////////////////////
	// ���з���
	HRESULT InitShader();
	void InitQuadTree();
	void SetDimesion( float width, float height/*���εļ��γߴ�*/, D3DXVECTOR2 centerPt/*���ε��е�λ��*/ );
	void CreateTerrain(CCamera* pCamera );
	void RenderTerrain(BOOL bFog );
	// ����X�����Y���꣬����õ�ĵ��θ߶�
	float GetTerrainElev( float x, float z );

	float GetSizeY() { return m_fHeight; }
	float GetSizeX() { return m_fWidth; }
	float GetPosX(){ return m_centerPt.x; }
	float GetPosY(){ return m_centerPt.y; }

protected:
	// ��ͷ���������һ����ȫ�Ķ�����
	void GrowQuadTree(CLODTrnNode* pNode);
	// ���ýڵ㼰�������ӽڵ�����ԣ�m_bReachAcceptableLevel��m_bTouched��m_bIsVisible���޸�Ϊfalse
	void ClearTreeFlag(CLODTrnNode* pNode);
	// �����Ĳ������жϽڵ�Ŀɼ��Ժ�����level�Ƿ�ﵽҪ�󣬲��Խڵ���б��
	void ClimbQuadTree(CLODTrnNode* pNode, LPDIRECT3DDEVICE9 pd3dDevice);
	// �ж�ĳ���ڵ��Ƿ�ɼ�
	BOOL IsVisible(CLODTrnNode* pNode, LPDIRECT3DDEVICE9 pd3dDevice);
	// �ж�ĳ���ڵ��level�Ƿ����㹻��
	BOOL IsReachAcceptableLevel(CLODTrnNode* pNode);
	// �õ��ڵ���ĸ����ڽڵ�ĸ��ڵ㣬�����ǵ�ָ��洢��һ��������
	void GetAdjNodeParent4(CLODTrnNode** pAdjNodeArray, CLODTrnNode* pNode);
	// �������λ��Ϊ(x, y)���Ǹ��ڵ�ĸ��ڵ�
	CLODTrnNode* FindParentNode(UINT x, UINT y);
	// �������λ��Ϊ(x, y)���Ǹ��ڵ�
	CLODTrnNode* FindNode(UINT x, UINT y);
	// �����Ĳ�����Ѱ������m_bReachAcceptableLevel == true�Ľڵ㣬�������з��������Ľڵ�д���������������������
	void RefineNode(CLODTrnNode* pNode);
protected:
	/*UINT m_height;*/
	UINT m_width;
	CLODTrnNode* m_pHeadNode;

	// ���εļ��γߴ缰����λ��
	float m_fWidth;
	float m_fHeight;

	float m_GridHeight; // �����ȣ��ӿ�����ٶ�
	float m_GridWidth;  // ����߶ȣ��ӿ�����ٶ�
	D3DXVECTOR2 m_centerPt;
	// ���Ӳü���
	CFrustum m_frustum;
	// �����
	CCamera* m_pCamera;

	// ��ά�߳�����
	float** m_elevArray;
	// ��ά������������
	D3DXVECTOR3 ** m_normalArray;
	// ������������
	float* m_tuArray;
	float* m_tvArray;
	// ��Ҫ��Ⱦ�ýڵ���
	UINT m_numNodeRendered;
	// ��������
	TRNVERTEX *m_vertexArray;
	// ��������
	WORD *m_indexArray;
	// ��������
	UINT m_vertexIndex;
	// ������������
	UINT m_indexIndex;
	// ���㻺��
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	// ��������
    LPDIRECT3DINDEXBUFFER9 m_pIB;

	// Shader
	IDirect3DVertexShader9* m_pVShader;
	IDirect3DPixelShader9 * m_pPShader;
	ID3DXConstantTable* m_pVConstTable;
	ID3DXConstantTable* m_pPConstTable;
	// Constant table
};

// ���ýڵ㼰�������ӽڵ�����ԣ�m_bReachAcceptableLevel��m_bTouched��m_bIsVisible���޸�Ϊfalse
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