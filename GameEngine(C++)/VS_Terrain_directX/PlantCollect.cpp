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
    // �쳣��⣬���û�д���½�أ����贴��ֲ��
    if(!g_pTerrain)
        return false;
    m_minHeight = minHeight;
    m_maxHeight = maxHeight;
    m_density = density;
    // ���ݼ���
    if(!LoadContent())
    {
        Release();
        return false;
    }
    // ����ֲ���ڵ�
    CreatePlantNode();
    // �������㻺�弰��������
    CreateVertices();
    return true;
}

bool CPlantCollect::LoadContent()
{
    // ��������
    HRESULT hr = D3DXCreateTextureFromFile(g_pDevice,"grass.png",&m_pGrassTexture);
    if(FAILED(hr))
        return false;
    return true;
}

void CPlantCollect::CreatePlantNode()
{
	

    // ��������½������
    for (int j=0;j<g_pTerrain->GetSizeY();j+=m_density)
    {
        for (int i=0;i<g_pTerrain->GetSizeX();i+=m_density)
        {
            // ������������һ�����ֵ���Է��ֲݱ�������� ^_^
            float posX = g_pTerrain->GetPosX() + i + rand()%(int)m_density;
            float posZ = g_pTerrain->GetPosY() + j + rand()%(int)m_density;
            // ���½���ϸõ�߶�
            float posY = g_pTerrain->GetTerrainElev(posX,posZ) ;
            // ����߶��������η�Χ֮�䣬�򴴽�ֲ���ڵ�
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
    // ���ֲ���ڵ���Ŀ
    int NodeNum = m_pPlantNodeList->size();
    // ����ö�������
    m_vertexNum = NodeNum * 12;
    // �������������
    m_indexNum  = NodeNum * 18;

    // �������㻺��
    g_pDevice->CreateVertexBuffer(
        m_vertexNum * sizeof(BILLBOARDVERTEX),
        D3DUSAGE_WRITEONLY,
        D3DFVF_BILLBOARDVERTEX,
        D3DPOOL_MANAGED,
        &m_pVB,
        0);

    BILLBOARDVERTEX* pVertices = NULL;
    m_pVB->Lock(0,0,(void**)&pVertices,0);
    
    // ���л�ȫ��ֲ���ڵ㶥��
    int vindex = 0;
    for(list<CBillboard>::iterator ptr = m_pPlantNodeList->begin(); ptr != m_pPlantNodeList->end(); ++ptr)
    {
        ptr->FlushVertexBuffer(pVertices,vindex);
    }

    m_pVB->Unlock();

    // ������������
    g_pDevice->CreateIndexBuffer(
        m_indexNum * sizeof(DWORD),
        D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX32,
        D3DPOOL_MANAGED,
        &m_pIB,
        0);

    DWORD* pIndices;
    m_pIB->Lock(0,0,(void**)&pIndices,0);

    // ���л�ȫ��ֲ���ڵ�����
    int index = 0;
    for(list<CBillboard>::iterator ptr = m_pPlantNodeList->begin(); ptr != m_pPlantNodeList->end(); ++ptr)
    {
        ptr->FlushIndexBuffer(pIndices,index);
    }

    m_pIB->Unlock();
}

void CPlantCollect::Draw(float gameTick)
{
    // ���ñ����޳�
    g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // ����Alphaͨ��
    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // ����Alpha���
    g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    g_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    g_pDevice->SetRenderState(D3DRS_ALPHAREF, 150);

    // �����������㻺�����������������������ʽ
    g_pDevice->SetTexture(0,m_pGrassTexture);
    g_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(BILLBOARDVERTEX));
    g_pDevice->SetIndices(m_pIB);
    g_pDevice->SetFVF(D3DFVF_BILLBOARDVERTEX);

    // ���ƶ���
    g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vertexNum, 0, m_indexNum/3);

    // �ر�Alpha���
    g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    // ����Alphaͨ��
    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

    // ���ñ����޳�
    g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

POINT CPlantCollect::GetPlantSize()
{
    // ���ֲ���ߴ�
    POINT Size;
    Size.x = 16;
    Size.y = 16;
    return Size;
    // ���Ҳ���������ﷵ��һ�����ֵ��ʹ�ô�����ֲ����С���� ^_^
}