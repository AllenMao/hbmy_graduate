#include ".\Camera.h"

CCamera::CCamera(void)
{
	this->look = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	this->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	this->right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	this->up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	velocity = 10.0f;
}

CCamera::~CCamera(void)
{
	
}

void CCamera::GetPos(D3DXVECTOR3 *pos)
{
	*pos = this->pos;
}

void CCamera::GetLook(D3DXVECTOR3 *look)
{
	*look = this->look;
}

void CCamera::Walk(float angle)
{
		pos += look * angle;
}

void CCamera::Fly(float angle)
{
		pos += up * angle;
}

void CCamera::Strafe(float angle)
{
		pos += right * angle;
}

void CCamera::Pitch(float angle)
{
	D3DXMATRIX t;
	D3DXMatrixRotationAxis(&t, &right, angle);
	D3DXVec3TransformCoord(&up, &up, &t);
	D3DXVec3TransformCoord(&look, &look, &t);
}

void CCamera::Yaw(float angle)
{
	D3DXMATRIX t;
	D3DXMatrixRotationAxis(&t, &up, angle);
	D3DXVec3TransformCoord(&right, &right, &t);
	D3DXVec3TransformCoord(&look, &look, &t);
}

void CCamera::Turn(float angle)
{
	D3DXMATRIX t;
	D3DXMatrixRotationY(&t,angle);
	D3DXVec3TransformCoord(&right, &right, &t);
	D3DXVec3TransformCoord(&look, &look, &t);

}

void CCamera::Jump(float time)
{
		
}

void CCamera::Roll(float angle)
{
	D3DXMATRIX t;
	D3DXMatrixRotationAxis(&t, &look, angle);
	D3DXVec3TransformCoord(&right, &right, &t);
	D3DXVec3TransformCoord(&up, &up, &t);
}

void CCamera::SetPos(D3DXVECTOR3 &pos)
{
	this->pos = pos;
}

void CCamera::SetLook(D3DXVECTOR3 &look)
{
	this->look = look;
}

void CCamera::SetUp(D3DXVECTOR3 &up)
{
	this->up = up;
}

void CCamera::SetCamera(IDirect3DDevice9 *device, float width, float height)
{
	D3DXVec3Normalize(&look, &look);

	D3DXVec3Cross(&up, &look, &right);
	D3DXVec3Normalize(&up, &up);

	D3DXVec3Cross(&right, &up, &look);
	D3DXVec3Normalize(&right, &right);

	float x = -D3DXVec3Dot(&right, &pos);
	float y = -D3DXVec3Dot(&up, &pos);
	float z = -D3DXVec3Dot(&look, &pos);
	tempMatrix._11 = right.x;	tempMatrix._12 = up.x;	tempMatrix._13 = look.x;	tempMatrix._14 = 0.0f;
	tempMatrix._21 = right.y;	tempMatrix._22 = up.y;	tempMatrix._23 = look.y;	tempMatrix._24 = 0.0f;
	tempMatrix._31 = right.z;	tempMatrix._32 = up.z;	tempMatrix._33 = look.z;	tempMatrix._34 = 0.0f;
	tempMatrix._41 = x;			tempMatrix._42 = y;		tempMatrix._43 = z;			tempMatrix._44 = 1.0f;
	device->SetTransform(D3DTS_VIEW, &tempMatrix);
	
	D3DXMatrixPerspectiveFovLH(&projectionMatrix,D3DX_PI*0.5f,(float)width/(float)height,0.1f,100000.0f);
	device->SetTransform(D3DTS_PROJECTION,&projectionMatrix);
}