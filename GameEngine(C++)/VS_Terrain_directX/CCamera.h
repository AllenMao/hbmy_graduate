#pragma once
#include "Common.h"

class CCamera
{
public:
	CCamera();
	~CCamera();

public:
	D3DXMATRIX GetViewMatrix();
	// �ı������״̬����������
	void Walk(float x, float z);
	void Pitch(float fAngle);// ���øú����ı�����ĸ�����
	void Yaw(float fYaw);    // ���øú����ı������ƫ����
	void GetLookVec(D3DXVECTOR3 *lookAtVec);
	float GetPosX()  { return m_x; }
	float GetPosY()  { return m_y; }
	float GetPosZ()  { return m_z; }
	void SetPosX(float x);
	void SetPosY(float y);
	void SetPosZ(float z);
protected:
	float m_x, m_y, m_z;   //�����λ��
	float m_pitchAngle;//������
	float m_yawAngle;  //ƫ����
};
