#pragma once

#include "Quad.h"
#include "Camera.h"

// 公告板类型		   面向视平面，面向摄像机，轴向
enum enBillboardType { BT_VIEW,    BT_CAMERA,  BT_AXIS };

class CBillboard
{
	CQuad *m_pPlane;
	/// 公告板位置
	D3DXVECTOR3 m_vPosition;
	/// 变换矩阵
	D3DXMATRIX	m_matWorld;
	enBillboardType m_enType;
	CCamera *camera;

public:
	CBillboard(void);
	~CBillboard(void);

	/** 初始化
		@param
			enType 公告板类型
	*/
	void Init(LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR2 &vSize, const char* pTexName, enBillboardType enType);
	void Update(CCamera *camera);
	void Render();
	/** 设置公告板位置
	*/
	void SetPosition(const D3DXVECTOR3& vPos);
	void SetType(enBillboardType enType);
	void SetSizeX(float fX);
};
