#include "Common.h"
#include "Billboard.h"

CBillboard::CBillboard(void) :
m_pPlane(NULL),
m_vPosition(0.0f, 0.0f, 0.0f),
m_enType(BT_VIEW)
{
}

CBillboard::~CBillboard(void)
{
	SafeDelete(m_pPlane);
}

void CBillboard::Init(LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR2 &vSize, const char* pTexName, enBillboardType enType)
{
	m_pPlane = new CQuad;
	m_pPlane->Init(pDevice, vSize.x, vSize.y, pTexName);
	m_enType = enType;
}

void CBillboard::Update(CCamera *camera)
{
	m_matWorld = camera->GetMatrix();
	m_matWorld._41 = m_matWorld._42 = m_matWorld._43 = 0.f;

	switch (m_enType)
	{
	case BT_VIEW:
		break;

	case BT_CAMERA:
		{
			D3DXVECTOR3 pos;
			camera->GetPos(&pos);
			D3DXVECTOR3 vDir = pos - m_vPosition;
			m_matWorld._13 = vDir.x;
			m_matWorld._23 = vDir.y;
			m_matWorld._33 = vDir.z;
		}
		break;

	case BT_AXIS:
		m_matWorld._12 = 0.f;
		m_matWorld._22 = 1.f;
		m_matWorld._32 = 0.f;	
		break;
	}

	D3DXMatrixTranspose( &m_matWorld, &m_matWorld );
	m_matWorld._41 = m_vPosition.x;
	m_matWorld._42 = m_vPosition.y;
	m_matWorld._43 = m_vPosition.z;
}

void CBillboard::Render()
{
	
	m_pPlane->Render(m_matWorld);
}

void CBillboard::SetPosition(const D3DXVECTOR3& vPos)
{
	m_vPosition = vPos;
}

void CBillboard::SetType(enBillboardType enType)
{
	m_enType = enType;
}

void CBillboard::SetSizeX(float fX)
{
	m_pPlane->SetSizeX(fX);
}
