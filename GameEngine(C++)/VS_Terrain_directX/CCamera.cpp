#include "CCamera.h"

CCamera::CCamera()
{
	m_x = m_y = m_z = 0;
	m_pitchAngle = 0;
	m_yawAngle = 0;
}

CCamera::~CCamera()
{
}

void CCamera::SetPosX(float x)
{
	m_x = x;
}

void CCamera::SetPosY(float y)
{
	m_y = y;
}

void CCamera::SetPosZ(float z)
{
	m_z = z;
}

void CCamera::GetLookVec(D3DXVECTOR3* m_vLookAtVec)
{
	float cosPitch = cos(m_pitchAngle);
	float sinPitch = sin(m_pitchAngle);
	float cosYaw   = cos(m_yawAngle);
	float sinYaw   = sin(m_yawAngle);

	m_vLookAtVec->x = cosPitch * sinYaw;
	m_vLookAtVec->y = sinPitch;
	m_vLookAtVec->z = cosPitch * cosYaw;
}

D3DXMATRIX CCamera::GetViewMatrix()
{
	D3DXMATRIX pMatView;
	D3DXVECTOR3 zAxis, m_vUp, xAxis, yAxis;
	float cosPitch = cos(m_pitchAngle);
	float sinPitch = sin(m_pitchAngle);
	float cosYaw   = cos(m_yawAngle);
	float sinYaw   = sin(m_yawAngle);
	// 构造zAxis,构造时已是单位向量
	zAxis.x = cosPitch * sinYaw;
	zAxis.y = sinPitch;
	zAxis.z = cosPitch * cosYaw;
	// 构造m_vUp,构造时已是单位向量
	m_vUp.x = -sinPitch * sinYaw;
	m_vUp.y = cosPitch;
	m_vUp.z = -sinPitch * cosYaw;
	// 构造xAxis和yAxis
	D3DXVec3Cross( &xAxis, &m_vUp, &zAxis );
	D3DXVec3Cross( &yAxis, &zAxis, &xAxis );

	// Build the view matrix:
	D3DXVECTOR3 m_vPos( m_x, m_y, m_z );
	float x = -D3DXVec3Dot(&xAxis, &m_vPos);
	float y = -D3DXVec3Dot(&yAxis, &m_vPos);
	float z = -D3DXVec3Dot(&zAxis, &m_vPos);

	(pMatView)(0,0) = xAxis.x; (pMatView)(0, 1) = yAxis.x; (pMatView)(0, 2) = zAxis.x; (pMatView)(0, 3) = 0.0f;
	(pMatView)(1,0) = xAxis.y; (pMatView)(1, 1) = yAxis.y; (pMatView)(1, 2) = zAxis.y; (pMatView)(1, 3) = 0.0f;
	(pMatView)(2,0) = xAxis.z; (pMatView)(2, 1) = yAxis.z; (pMatView)(2, 2) = zAxis.z; (pMatView)(2, 3) = 0.0f;
	(pMatView)(3,0) = x;       (pMatView)(3, 1) = y;       (pMatView)(3, 2) = z;       (pMatView)(3, 3) = 1.0f;
	
	return pMatView;
}

// +z代表前进方向，+x代表向右方向
void CCamera::Walk(float x, float z)
{
	float cosPitch = cos(m_pitchAngle);
	float sinPitch = sin(m_pitchAngle);
	float cosYaw   = cos(m_yawAngle);
	float sinYaw   = sin(m_yawAngle);

	m_z += z * cosPitch * cosYaw - x * cosPitch * sinYaw;
	m_x += z * cosPitch * sinYaw + x * cosPitch * cosYaw;
	m_y += z * sinPitch;
}

void CCamera::Pitch(float fAngle)
{
	m_pitchAngle += fAngle;
}

void CCamera::Yaw(float fAngle)
{
	m_yawAngle += fAngle;
}