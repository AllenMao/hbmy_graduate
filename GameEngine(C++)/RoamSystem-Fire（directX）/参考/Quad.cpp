#include "Common.h"
#include "Quad.h"

CQuad::CQuad(void) :
m_pDevice(NULL),
m_pMeshVertices(NULL),
m_pTexture(NULL),
m_nNumVertsAlongX(2),
m_nNumVertsAlongZ(2),
m_fMeshLengthAlongX(10.0f),
m_fMeshLengthAlongZ(10.0f),
m_nType(2)
{
}

CQuad::~CQuad(void)
{
	SafeRelease(m_pMeshVertices);
	SafeRelease(m_pTexture);
}

void CQuad::Init(LPDIRECT3DDEVICE9 pDevice, float fSizeX, float fSizeZ, const char* pTexName, int nType)
{
	m_nType = nType;
	m_pDevice = pDevice;

	m_fMeshLengthAlongX = fSizeX;
	m_fMeshLengthAlongZ = fSizeZ;

	// 创建纹理
	D3DXCreateTextureFromFile(m_pDevice, pTexName, &m_pTexture);

	m_pDevice->CreateVertexBuffer( 6*sizeof(QuadVertex),
								   D3DUSAGE_WRITEONLY, QuadVertex::FVF_Flags,
								   D3DPOOL_MANAGED, &m_pMeshVertices, NULL );

	QuadVertex *v;

	m_pMeshVertices->Lock( 0, 0, (void**)&v, 0 );

	for( int i = 0; i < 6; ++i )
	{
		v[i].ny = 1.0f;

		v[i].nx = 0.0f;
		v[i].ny = 1.0f;
		v[i].nz = 0.0f;

		v[i].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);
	}

	float fMinXPos = m_fMeshLengthAlongX * -0.5f;
	float fMaxXPos = m_fMeshLengthAlongX * 0.5f;
	float fMinZPos = m_fMeshLengthAlongZ * -0.5f;
	float fMaxZPos = m_fMeshLengthAlongZ * 0.5f;

	// 初始化
	// 1 ___ 2 4
	//  |  /|
	//  |/__|
	// 0 3   5

	// 0
	switch (nType)
	{
	case 0:		// X轴方向
		v[0].y = fMinXPos;
		v[0].z = fMinZPos;
		v[1].y = fMinXPos;
		v[1].z = fMaxZPos;
		v[2].y = fMaxXPos;
		v[2].z = fMaxZPos;
		v[3].y = fMinXPos; 
		v[3].z = fMinZPos;
		v[4].y = fMaxXPos;
		v[4].z = fMaxZPos;
		v[5].y = fMaxXPos;  
		v[5].z = fMinZPos;
		break;

	case 1:		// Y轴方向
		v[0].x = fMinXPos;
		v[0].z = fMinZPos;
		v[1].x = fMinXPos;
		v[1].z = fMaxZPos;
		v[2].x = fMaxXPos;
		v[2].z = fMaxZPos;
		v[3].x = fMinXPos; 
		v[3].z = fMinZPos;
		v[4].x = fMaxXPos;
		v[4].z = fMaxZPos;
		v[5].x = fMaxXPos;  
		v[5].z = fMinZPos;
		break;

	case 2:		// Z轴方向
		v[0].x = fMinXPos;
		v[0].y = fMinZPos;
		v[1].x = fMinXPos;
		v[1].y = fMaxZPos;
		v[2].x = fMaxXPos;
		v[2].y = fMaxZPos;
		v[3].x = fMinXPos; 
		v[3].y = fMinZPos;
		v[4].x = fMaxXPos;
		v[4].y = fMaxZPos;
		v[5].x = fMaxXPos;  
		v[5].y = fMinZPos;
		break;
	}

	v[0].tu = 0;
	v[0].tv = 1;
	v[1].tu = 0;
	v[1].tv = 0;
	v[2].tu = 1;
	v[2].tv = 0;
	v[3].tu = 0;
	v[3].tv = 1;
	v[4].tu = 1;
	v[4].tv = 0;
	v[5].tu = 1;
	v[5].tv = 1;

	m_pMeshVertices->Unlock();
}

void CQuad::Render( D3DXMATRIX& matWorld )
{
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);//设置源混合因子为（As,As,As,As)   
	m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);//设置目标混合因子为（1-As,1-As,1-As,1-As);   
	m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );//开启ALPHA测试功能   
	m_pDevice->SetRenderState( D3DRS_ALPHAREF,        0x0f );//设置ALPHA测试参考值   
	m_pDevice->SetRenderState( D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL );//设置APLHA测试比较规则
	m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_pDevice->SetStreamSource( 0, m_pMeshVertices, 0, sizeof(QuadVertex) );
	m_pDevice->SetFVF( QuadVertex::FVF_Flags );
	m_pDevice->SetTexture(0, m_pTexture);
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
}

void CQuad::SetSizeX(float fX)
{
	QuadVertex *v;
	m_pMeshVertices->Lock( 0, 0, (void**)&v, 0 );

	float fMaxXPos = fX;

	// 初始化
	// 1 ___ 2 4
	//  |  /|
	//  |/__|
	// 0 3   5

	// 0
	switch (m_nType)
	{
	case 0:		// X轴方向
		v[2].y = v[0].y + fMaxXPos;
		v[4].y = v[0].y + fMaxXPos;
		v[5].y = v[0].y + fMaxXPos;  
		break;

	case 1:		// Y轴方向
		v[2].x = v[0].x + fMaxXPos;
		v[4].x = v[0].x + fMaxXPos;
		v[5].x = v[0].x + fMaxXPos;  
		break;

	case 2:		// Z轴方向
		v[2].x = v[0].x + fMaxXPos;
		v[4].x = v[0].x + fMaxXPos;
		v[5].x = v[0].x + fMaxXPos;  
		break;
	}

	m_pMeshVertices->Unlock();
}
