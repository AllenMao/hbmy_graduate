#pragma once

#include "Common.h"

class CFrustum
{
public:
	CFrustum();
	~CFrustum();
public:
	// 构造裁减体的六个平面，每当视口改变的时候，都需要重新构造裁减体，再调用可视性检测函数
	void Construct( LPDIRECT3DDEVICE9 pd3dDevice );
	// 返回FALSE表示在裁减体之外（不可见），TRUE表示在裁减体之内（可见）
	// 点监测
	BOOL CheckPoint( D3DXVECTOR3 ptPos );
	// 立方体检测
	BOOL CheckCube( D3DXVECTOR3 centerPos, float size );
	// 矩形检测，与立方体检测的区别是，它默认矩形与 XOY 平面平行，不考虑垂直（Y）方向上的因素
	BOOL CheckRectangle( D3DXVECTOR3 centerPos, float size );
	// 检测球体
	BOOL CheckSphere( D3DXVECTOR3 centerPos, float radius );

protected:
	D3DXPLANE m_planes[6];
};