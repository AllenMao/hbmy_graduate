#include "Billboard.h"

CBillboard::CBillboard(IDirect3DDevice9 *pd3dDevice, D3DXVECTOR3 vCenterPos, int iWidth, int iHeight)
{
	m_pd3dDevice = pd3dDevice;
	m_pTreeVB = NULL;
	m_pTreeTex = NULL;

	m_vCenterPos = vCenterPos;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
}


CBillboard::~CBillboard(void)
{
	SAFE_RELEASE(m_pd3dDevice);
	SAFE_RELEASE(m_pTreeVB);
	SAFE_RELEASE(m_pTreeTex);
}

void CBillboard::Init(char *szFileName)
{
	if(szFileName != NULL)
		D3DXCreateTextureFromFile(m_pd3dDevice, szFileName, &m_pTreeTex);

	m_pd3dDevice->CreateVertexBuffer( 4 * sizeof(BILLBOARDVERTEX), 0, 
		D3DFVF_BILLBOARDVERTEX, D3DPOOL_MANAGED, &m_pTreeVB, NULL );
	BILLBOARDVERTEX *pVertices = NULL;

	   // 第一面各点偏移位置
    D3DXVECTOR3 pos_LU = D3DXVECTOR3(- m_iWidth / 2, + m_iHeight / 2, 0);
    D3DXVECTOR3 pos_RU = D3DXVECTOR3(+ m_iWidth / 2, + m_iHeight / 2, 0);
    D3DXVECTOR3 pos_LB = D3DXVECTOR3(- m_iWidth / 2, - m_iHeight / 2, 0);
    D3DXVECTOR3 pos_RB = D3DXVECTOR3(+ m_iWidth / 2, - m_iHeight / 2, 0);

    // 通用的纹理映射坐标
    D3DXVECTOR2 uv_LU = D3DXVECTOR2(0,0);
    D3DXVECTOR2 uv_RU = D3DXVECTOR2(1,0);
    D3DXVECTOR2 uv_LB = D3DXVECTOR2(0,1);
    D3DXVECTOR2 uv_RB = D3DXVECTOR2(1,1);

	m_pTreeVB->Lock(0, 0, (void**)&pVertices, 0); 
	pVertices[0] = BILLBOARDVERTEX(m_vCenterPos + pos_LB, uv_LB);
	pVertices[1] = BILLBOARDVERTEX(m_vCenterPos + pos_LU, uv_LU);
	pVertices[2] = BILLBOARDVERTEX(m_vCenterPos + pos_RB, uv_RB);
	pVertices[3] = BILLBOARDVERTEX(m_vCenterPos + pos_RU, uv_RU);

	m_pTreeVB->Unlock();

}

void CBillboard::Render(void)
{
	// 设置纹理状态
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	// Alpha混合设置, 设置混合系数
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   true );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	//D3DXMATRIXA16 matView;
	D3DXMATRIX matView;
	m_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	// 根据取景变换矩阵来构造广告牌矩阵
	D3DXMATRIX matBillboard;
	D3DXMatrixIdentity(&matBillboard);
	matBillboard._11 = matView._11;
	matBillboard._13 = matView._13;
	matBillboard._31 = matView._31;
	matBillboard._33 = matView._33;
	D3DXMatrixInverse(&matBillboard, NULL, &matBillboard);

	D3DXMATRIX matBillboard1;
	D3DXMatrixRotationY( &matBillboard1, D3DX_PI);

	D3DXMATRIX matTree;
	D3DXMatrixIdentity(&matTree);
	matTree = matBillboard * matTree * matBillboard1;
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTree );

	// 渲染树木
	m_pd3dDevice->SetTexture( 0, m_pTreeTex );
	m_pd3dDevice->SetStreamSource( 0, m_pTreeVB, 0, sizeof(BILLBOARDVERTEX) );
	m_pd3dDevice->SetFVF( D3DFVF_BILLBOARDVERTEX );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0, 2 );

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   false );
	D3DXMatrixIdentity(&matTree);
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTree );
}

float GetRandomFloat(float min, float max)
{
    return min + rand()%((int)max + 1 - (int)min);
}

void CBillboard::FlushVertexBuffer(BILLBOARDVERTEX* pVertices, int& index)
{
    // 第一面各点偏移位置
    D3DXVECTOR3 pos_LU = D3DXVECTOR3(- m_iWidth / 2, + m_iHeight / 2, 0);
    D3DXVECTOR3 pos_RU = D3DXVECTOR3(+ m_iWidth / 2, + m_iHeight / 2, 0);
    D3DXVECTOR3 pos_LB = D3DXVECTOR3(- m_iWidth / 2, - m_iHeight / 2, 0);
    D3DXVECTOR3 pos_RB = D3DXVECTOR3(+ m_iWidth / 2, - m_iHeight / 2, 0);

    // 通用的纹理映射坐标
    D3DXVECTOR2 uv_LU = D3DXVECTOR2(0,0);
    D3DXVECTOR2 uv_RU = D3DXVECTOR2(1,0);
    D3DXVECTOR2 uv_LB = D3DXVECTOR2(0,1);
    D3DXVECTOR2 uv_RB = D3DXVECTOR2(1,1);

    // 产生一个随机的偏转角，使得植被看起来形状各异
    float randomAngle = GetRandomFloat(D3DX_PI/4.0f, D3DX_PI*5.0f/12.0f);
    D3DXMATRIX rotateMatrix;
    D3DXMatrixRotationY(&rotateMatrix, randomAngle);
    D3DXMATRIX rotateMatrix2;
    D3DXMatrixRotationY(&rotateMatrix2, D3DX_PI - randomAngle);

    // 第二面各点偏移位置
    D3DXVECTOR3 pos_LU2;
    D3DXVECTOR3 pos_RU2;
    D3DXVECTOR3 pos_LB2;
    D3DXVECTOR3 pos_RB2;

    D3DXVec3TransformCoord(&pos_LU2, &pos_LU, &rotateMatrix);
    D3DXVec3TransformCoord(&pos_RU2, &pos_RU, &rotateMatrix);
    D3DXVec3TransformCoord(&pos_LB2, &pos_LB, &rotateMatrix);
    D3DXVec3TransformCoord(&pos_RB2, &pos_RB, &rotateMatrix);

    // 第三面各点偏移位置
    D3DXVECTOR3 pos_LU3;
    D3DXVECTOR3 pos_RU3;
    D3DXVECTOR3 pos_LB3;
    D3DXVECTOR3 pos_RB3;

    D3DXVec3TransformCoord(&pos_LU3, &pos_LU, &rotateMatrix2);
    D3DXVec3TransformCoord(&pos_RU3, &pos_RU, &rotateMatrix2);
    D3DXVec3TransformCoord(&pos_LB3, &pos_LB, &rotateMatrix2);
    D3DXVec3TransformCoord(&pos_RB3, &pos_RB, &rotateMatrix2);

    // 各面顶点序列化到顶点缓冲
    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_LU, uv_LU);
    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_RU, uv_RU);
    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_LB, uv_LB);
    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_RB, uv_RB);

    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_LU2, uv_LU);
    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_RU2, uv_RU);
    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_LB2, uv_LB);
    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_RB2, uv_RB);

    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_LU3, uv_LU);
    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_RU3, uv_RU);
    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_LB3, uv_LB);
    pVertices[index++] = BILLBOARDVERTEX(m_vCenterPos + pos_RB3, uv_RB);
}

void CBillboard::FlushIndexBuffer(DWORD* pIndices, int& index)
{
    // 每面4个顶点对应6个索引，其比率为2：3
    DWORD vindex = (DWORD)index * 2 / 3;

    // 序列化第一面各点索引
    pIndices[index++] = vindex;
    pIndices[index++] = vindex + 1;
    pIndices[index++] = vindex + 2;

    pIndices[index++] = vindex + 1;
    pIndices[index++] = vindex + 3;
    pIndices[index++] = vindex + 2;

    // 序列化第二面各点索引
    pIndices[index++] = vindex + 4;
    pIndices[index++] = vindex + 5;
    pIndices[index++] = vindex + 6;

    pIndices[index++] = vindex + 5;
    pIndices[index++] = vindex + 7;
    pIndices[index++] = vindex + 6;

    // 序列化第三面各点索引
    pIndices[index++] = vindex + 8;
    pIndices[index++] = vindex + 9;
    pIndices[index++] = vindex + 10;

    pIndices[index++] = vindex + 9;
    pIndices[index++] = vindex + 11;
    pIndices[index++] = vindex + 10;
}
