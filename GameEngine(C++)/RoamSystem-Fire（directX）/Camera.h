#pragma once
#include "Common.h"
class CCamera
{
public:
	CCamera( IDirect3DDevice9* Device );
	~CCamera(void);
	void Walk(float angle);				//����: ��look�����ƶ�     �̶�����look������Zƽ��ķ��������ƶ�
	void Fly(float angle);				//����: ��up�����ƶ�		  �̶�: ��Y���ƶ�
	void Strafe(float angle);			//����: ��right�����ƶ�     �̶�����right������Zƽ��ķ��������ƶ�

	void Pitch(float angle);		//��right������ת
	void Yaw(float angle);		//����: ��up������ת     �̶�����Y����ת
	void Roll(float angle);		//����: ��look������ת   �̶�ģʽ�²�����

	void Turn(float angle);
	void Jump(float time);

	void SetCamera( float width, float height );	//���������
	inline D3DXMATRIX GetMatrix(){return tempMatrix;}
	inline D3DXMATRIX GetProjection(){return projectionMatrix;}

	void SetPos(D3DXVECTOR3 &pos);											//���������λ��
	void GetPos(D3DXVECTOR3 *pos);											//��ȡ�����λ��
	void GetLook(D3DXVECTOR3 *look);										//��ȡ������۲췽��
	void SetLook(D3DXVECTOR3 &look);										//��ȡ������۲췽��
	void SetUp(D3DXVECTOR3 &up);											//������������ϵ���
private:
	IDirect3DDevice9* m_pd3dDevice;
	D3DXMATRIX tempMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXVECTOR3 up;
	D3DXVECTOR3 look;
	D3DXVECTOR3 right;
	D3DXVECTOR3 pos;
	bool jumpping;
	float velocity;
};