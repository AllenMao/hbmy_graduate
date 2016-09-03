#include "PlantCollect.h"
#include "CTerrain.h"


extern IDirect3DDevice9 *g_pDevice;
extern CTerrain *g_pTerrain;

CPlantCollect::CPlantCollect(void) : m_pPlantNodeList(NULL),
                                     m_pGrassTexture(NULL),
                                     m_pVB(NULL),
                                     m_pIB(NULL),
                                     m_minHeight(0),
                                     m_maxHeight(0),
                                     m_density(0),
                                     m_vertexNum(0),
                                     m_indexNum(0)
{
    m_pPlantNodeList = new list<CBillboard>;
}

CPlantCollect::~CPlantCollect(void)
{
}

void CPlantCollect::Release()
{
	SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pIB);
    SAFE_RELEASE(m_pGrassTexture);
    if(m_pPlantNodeList->size()>0)
        m_pPlantNodeList->clear();
    delete m_pPlantNodeList;
}

bool CPlantCollect::Create(float minHeight, float maxHeight, float density)
{
    // 异常检测，如果没有创建陆地，则不予创建植被
    if(!g_pTerrain)
        return false;
    m_minHeight = minHeight;
    m_maxHeight = maxHeight;
    m_density = density;
    // 内容加载
    if(!LoadContent())
    {
        Release();
        return false;
    }
    // 创建植被节点
    CreatePlantNode();
    // 创建顶点缓冲及索引缓冲
    CreateVertices();
    return true;
}

bool CPlantCollect::LoadContent()
{
    // 创建纹理
    HRESULT hr = D3DXCreateTextureFromFile(g_pDevice,"grass.png",&m_pGrassTexture);
    if(FAILED(hr))
        return false;
    return true;
}

void CPlantCollect::CreatePlantNode()
{
	

    // 遍历整个陆地区域
    for (int j=0;j<g_pTerrain->GetSizeY();j+=m_density)
    {
        for (int i=0;i<g_pTerrain->GetSizeX();i+=m_density)
        {
            // 横纵坐标各添加一个随机值，以防种草变成种麦子 ^_^
            float posX = g_pTerrain->GetPosX() + i + rand()%(int)m_density;
            float posZ = g_pTerrain->GetPosY() + j + rand()%(int)m_density;
            // 获得陆地上该点高度
            float posY = g_pTerrain->GetTerrainElev(posX,posZ) ;
            // 如果高度在允许海拔范围之间，则创建植被节点
            if(posY >= m_minHeight && posY <= m_maxHeight)
            {
                D3DXVECTOR3 pos = D3DXVECTOR3(posX, posY, posZ);
                POINT Size = GetPlantSize();
                pos.y += Size.y/2;
                m_pPlantNodeList->push_back(CBillboard(NULL, pos, Size.x, Size.y));

				//puts("add success");
            }
        }
    }
}

void CPlantCollect::CreateVertices()
{
    // 获得植被节点数目
    int NodeNum = m_pPlantNodeList->size();
    // 计算得顶点总数
    m_vertexNum = NodeNum * 12;
    // 计算得索引总数
    m_indexNum  = NodeNum * 18;

    // 创建顶点缓冲
    g_pDevice->CreateVertexBuffer(
        m_vertexNum * sizeof(BILLBOARDVERTEX),
        D3DUSAGE_WRITEONLY,
        D3DFVF_BILLBOARDVERTEX,
        D3DPOOL_MANAGED,
        &m_pVB,
        0);

    BILLBOARDVERTEX* pVertices = NULL;
    m_pVB->Lock(0,0,(void**)&pVertices,0);
    
    // 序列化全部植被节点顶点
    int vindex = 0;
    for(list<CBillboard>::iterator ptr = m_pPlantNodeList->begin(); ptr != m_pPlantNodeList->end(); ++ptr)
    {
        ptr->FlushVertexBuffer(pVertices,vindex);
    }

    m_pVB->Unlock();

    // 创建索引缓冲
    g_pDevice->CreateIndexBuffer(
        m_indexNum * sizeof(DWORD),
        D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX32,
        D3DPOOL_MANAGED,
        &m_pIB,
        0);

    DWORD* pIndices;
    m_pIB->Lock(0,0,(void**)&pIndices,0);

    // 序列化全部植被节点索引
    int index = 0;
    for(list<CBillboard>::iterator ptr = m_pPlantNodeList->begin(); ptr != m_pPlantNodeList->end(); ++ptr)
    {
        ptr->FlushIndexBuffer(pIndices,index);
    }

    m_pIB->Unlock();
}

void CPlantCollect::Draw(float gameTick)
{
    // 禁用背面剔除
    g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // 启用Alpha通道
    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // 开启Alpha检测
    g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    g_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    g_pDevice->SetRenderState(D3DRS_ALPHAREF, 150);

    // 设置纹理、顶点缓冲区、索引缓冲区及顶点格式
    g_pDevice->SetTexture(0,m_pGrassTexture);
    g_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(BILLBOARDVERTEX));
    g_pDevice->SetIndices(m_pIB);
    g_pDevice->SetFVF(D3DFVF_BILLBOARDVERTEX);

    // 绘制顶点
    g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vertexNum, 0, m_indexNum/3);

    // 关闭Alpha检测
    g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    // 禁用Alpha通道
    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

    // 重用背面剔除
    g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

POINT CPlantCollect::GetPlantSize()
{
    // 获得植被尺寸
    POINT Size;
    Size.x = 16;
    Size.y = 16;
    return Size;
    // 大家也可以在这里返回一个随机值，使得创建的植被大小各异 ^_^
}