#pragma once
#include "Common.h"

class CCamera
{
public:
	CCamera();
	~CCamera();

public:
	D3DXMATRIX GetViewMatrix();
	// 改变摄像机状态的三个函数
	void Walk(float x, float z);
	void Pitch(float fAngle);// 调用该函数改变相机的俯仰角
	void Yaw(float fYaw);    // 调用该函数改变相机的偏航角
	void GetLookVec(D3DXVECTOR3 *lookAtVec);
	float GetPosX()  { return m_x; }
	float GetPosY()  { return m_y; }
	float GetPosZ()  { return m_z; }
	void SetPosX(float x);
	void SetPosY(float y);
	void SetPosZ(float z);
protected:
	float m_x, m_y, m_z;   //摄像机位置
	float m_pitchAngle;//俯仰角
	float m_yawAngle;  //偏航角
};
