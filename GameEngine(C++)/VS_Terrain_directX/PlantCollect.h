#pragma once

#include "Billboard.h"
#include "Common.h"

using namespace std;

class CPlantCollect
{
public:
    CPlantCollect(void);
    ~CPlantCollect(void);
public:
    bool Create(                                  // 生成植被
        float minHeight,                          // 最低海拔
        float maxHeight,                          // 最高海拔
        float density);                           // 密度(间距)
    void Draw(float gameTick);                    // 绘制植被
    void Release();                               // 资源释放
private:
    bool  LoadContent();                          // 加载资源
    void  CreateVertices();                       // 生成顶点及索引缓冲
    void  CreatePlantNode();                      // 生成植被节点
    POINT GetPlantSize();                         // 获得植被尺寸
private:
    list<CBillboard>* m_pPlantNodeList;          // 植被节点列表
    IDirect3DTexture9* m_pGrassTexture;           // 草地纹理
    IDirect3DVertexBuffer9*  m_pVB;               // 顶点缓冲
    IDirect3DIndexBuffer9*   m_pIB;               // 索引缓冲
private:
    float m_minHeight;                            // 最低海拔
    float m_maxHeight;                            // 最高海拔
    float m_density;                              // 密度
    int   m_vertexNum;                            // 顶点总数
    int   m_indexNum;                             // 索引总数
};