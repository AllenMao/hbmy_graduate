#pragma once
#include "Common.h"
class CCamera
{
public:
	CCamera( IDirect3DDevice9* Device );
	~CCamera(void);
	void Walk(float angle);				//自由: 沿look方向移动     固定：沿look方向在Z平面的分量方向移动
	void Fly(float angle);				//自由: 沿up方向移动		  固定: 沿Y轴移动
	void Strafe(float angle);			//自由: 沿right方向移动     固定：沿right方向在Z平面的分量方向移动

	void Pitch(float angle);		//绕right方向旋转
	void Yaw(float angle);		//自由: 绕up方向旋转     固定：绕Y轴旋转
	void Roll(float angle);		//自由: 绕look方向旋转   固定模式下不可用

	void Turn(float angle);
	void Jump(float time);

	void SetCamera( float width, float height );	//设置摄像机
	inline D3DXMATRIX GetMatrix(){return tempMatrix;}
	inline D3DXMATRIX GetProjection(){return projectionMatrix;}

	void SetPos(D3DXVECTOR3 &pos);											//设置摄像机位置
	void GetPos(D3DXVECTOR3 *pos);											//获取摄像机位置
	void GetLook(D3DXVECTOR3 *look);										//获取摄像机观察方向
	void SetLook(D3DXVECTOR3 &look);										//获取摄像机观察方向
	void SetUp(D3DXVECTOR3 &up);											//设置摄像机向上的轴
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