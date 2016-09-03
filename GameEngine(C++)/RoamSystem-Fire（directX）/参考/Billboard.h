#pragma once

#include "Quad.h"
#include "Camera.h"

// ���������		   ������ƽ�棬���������������
enum enBillboardType { BT_VIEW,    BT_CAMERA,  BT_AXIS };

class CBillboard
{
	CQuad *m_pPlane;
	/// �����λ��
	D3DXVECTOR3 m_vPosition;
	/// �任����
	D3DXMATRIX	m_matWorld;
	enBillboardType m_enType;
	CCamera *camera;

public:
	CBillboard(void);
	~CBillboard(void);

	/** ��ʼ��
		@param
			enType ���������
	*/
	void Init(LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR2 &vSize, const char* pTexName, enBillboardType enType);
	void Update(CCamera *camera);
	void Render();
	/** ���ù����λ��
	*/
	void SetPosition(const D3DXVECTOR3& vPos);
	void SetType(enBillboardType enType);
	void SetSizeX(float fX);
};
