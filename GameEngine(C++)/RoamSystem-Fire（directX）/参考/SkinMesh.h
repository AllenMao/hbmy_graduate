//////////////////////////////////////////////////////////////////////
// SkinMesh.h: interface for the CSkinMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINMESH_H)
#define AFX_SKINMESH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tchar.h>
#include <d3dx9.h>

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//-----------------------------------------------------------------------------
// Name: struct D3DXFRAME_DERIVED
// Desc: Structure derived from D3DXFRAME so we can add some app-specific
//       info that will be stored with each frame
//-----------------------------------------------------------------------------
struct D3DXFRAME_DERIVED: public D3DXFRAME
{
    D3DXMATRIXA16        CombinedTransformationMatrix;
};

//-----------------------------------------------------------------------------
// Name: struct D3DXMESHCONTAINER_DERIVED
// Desc: Structure derived from D3DXMESHCONTAINER so we can add some app-specific
//       info that will be stored with each mesh
//-----------------------------------------------------------------------------
struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
    LPDIRECT3DTEXTURE9*  ppTextures;       // array of textures, entries are NULL if no texture specified    
                                
    // SkinMesh info             
    LPD3DXMESH           pOrigMesh;
    LPD3DXATTRIBUTERANGE pAttributeTable;
    DWORD                NumAttributeGroups; 
    DWORD                NumInfl;
    LPD3DXBUFFER         pBoneCombinationBuf;
    D3DXMATRIX**         ppBoneMatrixPtrs;
    D3DXMATRIX*          pBoneOffsetMatrices;
    DWORD                NumPaletteEntries;
    bool                 UseSoftwareVP;
    DWORD                iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning
};

// enum for various skinning modes possible
enum METHOD 
{
    D3DNONINDEXED,
    D3DINDEXED,
    SOFTWARE,
    NONE
};

class CSkinMesh;

//-----------------------------------------------------------------------------
// Name: class CAllocateHierarchy
// Desc: Custom version of ID3DXAllocateHierarchy with custom methods to create
//       frames and meshcontainers.
//-----------------------------------------------------------------------------

//这里是Directx9.0a的
class CAllocateHierarchy: public ID3DXAllocateHierarchy
{
public:
    STDMETHOD(CreateFrame)(THIS_ LPCTSTR Name, LPD3DXFRAME *ppNewFrame);

#if (D3D_SDK_VERSION &31)	//这里是Directx9.0b的
    STDMETHOD(CreateMeshContainer)(THIS_ LPCTSTR Name, 
							LPD3DXMESHDATA pMeshData,
                            LPD3DXMATERIAL pMaterials, 
							LPD3DXEFFECTINSTANCE pEffectInstances, 
							DWORD NumMaterials, 
                            DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, 
                            LPD3DXMESHCONTAINER *ppNewMeshContainer);
#else						//这里是Directx9.0c的
    STDMETHOD(CreateMeshContainer)(THIS_ 
        LPCSTR Name, 
        CONST D3DXMESHDATA *pMeshData,
        CONST D3DXMATERIAL *pMaterials, 
        CONST D3DXEFFECTINSTANCE *pEffectInstances, 
        DWORD NumMaterials, 
        CONST DWORD *pAdjacency, 
        LPD3DXSKININFO pSkinInfo, 
        LPD3DXMESHCONTAINER *ppNewMeshContainer);
#endif

    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
    CAllocateHierarchy(CSkinMesh *pSkinMesh) :m_pSkinMesh(pSkinMesh) {}

public:
	CSkinMesh*		m_pSkinMesh;
};

//这里是DirectX9.0C的



//蒙皮动画
class CSkinMesh  
{
public:
	VOID SetAnim(BOOL bAnim);
	BOOL CSkinMesh::InterSect( D3DVECTOR *pRayOrig,
		 D3DVECTOR *pRayDir,D3DVECTOR* pVRet);
	VOID UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix );

	LPDIRECT3DVERTEXSHADER9 m_pIndexedVertexShader[4];
	D3DXMATRIXA16* m_pBoneMatrices;

	UINT		m_NumBoneMatricesMax;
	METHOD		m_SkinningMethod;
	D3DCAPS9 					m_d3dCaps;
    D3DXVECTOR3                 m_vObjectCenter;    // Center of bounding sphere of object
    FLOAT                       m_fObjectRadius;    // Radius of bounding sphere of object
	D3DXVECTOR3		m_vMin;
	D3DXVECTOR3		m_vMax;


	//考虑到模型的重复引用,这里可以定义不使用模型自身的坐标,比例和角度
#ifndef NO_USE_LOCALPOSITION
	FLOAT						m_fAngle;
	FLOAT						m_fScale;
	D3DXVECTOR3					m_vPos;
	D3DXVECTOR3					m_vPosOld;


	VOID SetPosition(D3DXVECTOR3 *pPos)		{m_vPos=*pPos;};
	VOID SetPosition(float x,float y,float z) {m_vPos.x=x;m_vPos.y=y;m_vPos.z=z;};
	VOID SetDirection(float fAngle){m_fAngle=fAngle;};
	VOID SetScale(float s){m_fScale=s;};

	VOID GetPosition(D3DXVECTOR3 *vPos) 
	{
		*vPos=m_vPos;
	}
	const	LPD3DXVECTOR3 GetPosition()
	{
		return &m_vPos;
	
	}

	VOID GetDirection(D3DXVECTOR3 *vDir) 
	{	vDir->x=sinf(m_fAngle);
		vDir->y=0;
		vDir->z=cosf(m_fAngle);
	};
	VOID Render(float fElapsedAppTime)
	{
		Render(fElapsedAppTime,m_vPos,m_fAngle,m_fScale);
	};

	VOID GetBoundingBox(D3DXVECTOR3 *vMin,D3DXVECTOR3 *vMax)
	{	*vMin=m_vPos+m_vMin*m_fScale;
		*vMax=m_vPos+m_vMax*m_fScale;
	}

	VOID Turn(float fAngle)
	{
		m_fAngle+=fAngle;  
	}
	VOID WalkFoward(float fDistance)
	{
		m_vPosOld=m_vPos;  
		D3DXVECTOR3 vDir;
		GetDirection(&vDir);
		m_vPos+=fDistance*vDir;
	}

	VOID Back()
	{
		m_vPos=m_vPosOld; 
	
	}
	VOID SetAltitude(float y)
	{
		m_vPos.y=y; 
	}


	LRESULT CollisionTestAABB(CSkinMesh *pMesh)
	{
		D3DXVECTOR3 A1,A2;
		D3DXVECTOR3 B1,B2;
		GetBoundingBox(&A1,&A2);  
		pMesh->GetBoundingBox(&B1,&B2);

		return (A1.x<B2.x && A2.x>B1.x
			 && A1.y<B2.y && A2.y>B1.y
			 && A1.z<B2.z && A2.z>B1.z);		
	
	}

#endif

	LPD3DXANIMATIONCONTROLLER GetAnimController()
		{return m_pAnimController;};

	BOOL SetAnimationName(char *strAnimName);

	VOID	Render(float fTimeFromLastFrame,D3DXVECTOR3 vPos,float angle,float scale=1.0f);
	HRESULT LoadFromXFile(char* strFileName);
	HRESULT GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer);
	HRESULT CalculateStaticBondingBox(LPD3DXFRAME pFrameBase, const LPD3DXVECTOR3 pVmin,const LPD3DXVECTOR3 pVmax);


	CSkinMesh(LPDIRECT3DDEVICE9 pD3DDevice);
//	CSkinMesh(LPDIRECT3DDEVICE9 pD3DDevice,char* strFileName);

	virtual ~CSkinMesh();
protected:
//	FLOAT						m_fRotateAngle;
	BOOL						m_bMoving;
	LPDIRECT3DDEVICE9			m_pd3dDevice;
	LPD3DXFRAME					m_pFrameRoot;
	LPD3DXANIMATIONCONTROLLER	m_pAnimController;
    FLOAT						m_fElapsedTime;      // Time elapsed since last frame


	VOID DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);
	VOID DrawFrame(LPD3DXFRAME pFrame);
	HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );
	HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );
};

#endif // !defined(AFX_SKINMESH_H__AE3ADAAF_4BE2_4B23_9A1D_9379FA1BAFD9__INCLUDED_)
