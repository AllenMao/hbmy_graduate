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
    bool Create(                                  // ����ֲ��
        float minHeight,                          // ��ͺ���
        float maxHeight,                          // ��ߺ���
        float density);                           // �ܶ�(���)
    void Draw(float gameTick);                    // ����ֲ��
    void Release();                               // ��Դ�ͷ�
private:
    bool  LoadContent();                          // ������Դ
    void  CreateVertices();                       // ���ɶ��㼰��������
    void  CreatePlantNode();                      // ����ֲ���ڵ�
    POINT GetPlantSize();                         // ���ֲ���ߴ�
private:
    list<CBillboard>* m_pPlantNodeList;          // ֲ���ڵ��б�
    IDirect3DTexture9* m_pGrassTexture;           // �ݵ�����
    IDirect3DVertexBuffer9*  m_pVB;               // ���㻺��
    IDirect3DIndexBuffer9*   m_pIB;               // ��������
private:
    float m_minHeight;                            // ��ͺ���
    float m_maxHeight;                            // ��ߺ���
    float m_density;                              // �ܶ�
    int   m_vertexNum;                            // ��������
    int   m_indexNum;                             // ��������
};