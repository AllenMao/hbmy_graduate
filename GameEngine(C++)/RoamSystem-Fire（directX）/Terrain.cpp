#include "Terrain.h"

CTerrain::CTerrain( IDirect3DDevice9* pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	m_pTexture = NULL;
	m_pIndexBuffer = NULL;
	m_pVertexBuffer = NULL;
	m_iCellsPerRow = 0;
	m_iCellsPerCol = 0;
	m_iVertsPerRow = 0;
	m_iVertsPerCol = 0;
	m_iNumVertices = 0;
	m_fTerrainWidth = 0.0f;		//���εĿ�� 
    m_fTerrainDepth = 0.0f;		//���ε���� 
    m_fCellSpacing = 0.0f;		//��Ԫ���ļ��
    m_fHeightScale = 0.0f;		//�߶�����ϵ�� 
}

CTerrain::~CTerrain(void)
{
	m_pVertexBuffer->Release();
	m_pIndexBuffer->Release();
}

float CTerrain::GetCameraHeight(float x, float z)
{
	//����ͼ�ƶ���ԭ��,�������
	x = ((float) m_fTerrainWidth / 2.0f) + x;
	z = ((float) m_fTerrainDepth / 2.0f) - z;
	//����ͼ��С��λ�����������
	x /= (float)m_fCellSpacing;
	z /= (float)m_fCellSpacing;
	//����x,z�������ڵ�����ֵ
	float col = ::floorf(x);
	float row = ::floorf(z);

	// ��ȡ����ͼ4������ĸ߶�
	// 
	//  A   B
	//  *---*
	//  | / |
	//  *---*  
	//  C   D

	float A = GetHeight((int)row,   (int)col);
	float B = GetHeight((int)row,   (int)col+1);
	float C = GetHeight((int)row+1, (int)col);
	float D = GetHeight((int)row+1, (int)col+1);
	
	float dx = x - col;
	float dz = z - row;


	float height = 0.0f;
	if(dz < 1.0f - dx)//(x,z)����ABC��������
	{
		float uy = B - A;
		float vy = C - A;

		height = A + Lerp(0.0f, uy, dx) + Lerp(0.0f, vy, dz);//���β�ֵ�õ��߶�
	}
	else//(x,z)����BCD��������
	{
		float uy = C - D;
		float vy = B - D;

		height = D + Lerp(0.0f, uy, 1.0f - dx) + Lerp(0.0f, vy, 1.0f - dz);
	}

	return height;
}

float CTerrain::Lerp(float a, float b, float t)
{
	return a - (a*t) + (b*t);
}

BOOL CTerrain::LoadTerrainFromFile( char* pRawFileName, char* pTextureFile, char* pTextureFile1 )
{
	ifstream inFile;
	inFile.open( pRawFileName, ios::binary );

	inFile.seekg( 0,ios::end );					//���ļ�ָ���ƶ����ļ�ĩβ

    //��ģ�嶨��һ��vector<BYTE>���͵ı���inData����ʼ������ֵΪ��������ǰλ�ã�����������С
	vector<BYTE> inData( inFile.tellg() );
	inFile.seekg( ios::beg );			//���ļ�ָ���ƶ����ļ��Ŀ�ͷ��׼����ȡ�߶���Ϣ
    inFile.read( (char*)&inData[0], inData.size() );	//�ؼ���һ������ȡ�����߶���Ϣ
    inFile.close();	

	m_vHeightInfo.resize(inData.size());	//��m_vHeightInfo�ߴ�ȡΪ�������ĳߴ�
	for (unsigned int i=0; i<inData.size(); i++)		
        m_vHeightInfo[i] = inData[i];
	// ���ص�������
    if (FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, pTextureFile, &m_pTexture)))
        return FALSE;
	if (FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, pTextureFile1, &m_pTexture1)))
        return FALSE;

    return TRUE;
}

BOOL CTerrain::InitTerrain( INT iRows, INT iCols, FLOAT fSpace, FLOAT fScale )
{
	m_iCellsPerRow  = iRows;  //ÿ�еĵ�Ԫ����Ŀ
    m_iCellsPerCol  = iCols;  //ÿ�еĵ�Ԫ����Ŀ
    m_fCellSpacing  = fSpace;	//��Ԫ���ļ��
    m_fHeightScale  = fScale; //�߶�����ϵ��
    m_fTerrainWidth = iRows * fSpace;  //���εĿ��
    m_fTerrainDepth = iCols * fSpace;  //���ε����
    m_iVertsPerRow  = m_iCellsPerCol + 1;  //ÿ�еĶ�����
    m_iVertsPerCol  = m_iCellsPerRow + 1; //ÿ�еĶ�����
    m_iNumVertices  = m_iVertsPerRow * m_iVertsPerCol;  //��������
	
	for( unsigned int i = 0; i<m_vHeightInfo.size(); ++i )
	{
		/*printf( "%lf       ", m_vHeightInfo[i] );
		if( i % 255 == 0 && i != 0 ) puts("end");*/
		m_vHeightInfo[i] *= m_fHeightScale;
	}

	if( FAILED( m_pd3dDevice->CreateVertexBuffer( m_iNumVertices * sizeof(TERRAINVERTEX), 
											D3DUSAGE_WRITEONLY, TERRAINVERTEX::FVF, D3DPOOL_MANAGED, &m_pVertexBuffer, 0 ) ) )
	{
		return FALSE;
	}

	TERRAINVERTEX* pVertices = NULL;
	m_pVertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );

	FLOAT fStartX = -m_fTerrainWidth / 2.0f, fEndX =  m_fTerrainWidth / 2.0f;		//ָ����ʼ��ͽ������X����ֵ
    FLOAT fStartZ =  m_fTerrainDepth / 2.0f, fEndZ = -m_fTerrainDepth / 2.0f;	//ָ����ʼ��ͽ������Z����ֵ
    FLOAT fCoordU = 80.0f / (FLOAT)m_iCellsPerRow;     //ָ������ĺ�����ֵ
    FLOAT fCoordV = 80.0f / (FLOAT)m_iCellsPerCol;		//ָ�������������ֵ

	int iIndex = 0, i = 0, j = 0;
	for( float z = fStartZ; z > fEndZ; z -= m_fCellSpacing, ++i )
	{
		j = 0;
		for ( float x = fStartX; x < fEndX; x += m_fCellSpacing, ++j )	//X���귽������ʼ���㵽���������м�ı���
        {
            iIndex = i * m_iCellsPerRow + j;		//ָ����ǰ�����ڶ��㻺���е�λ�� 
            pVertices[iIndex] = TERRAINVERTEX(x, m_vHeightInfo[iIndex], z, 0.0f, 0.0f, 0.0f, j*fCoordU, i*fCoordV, j*fCoordU, i*fCoordV ); //�Ѷ���λ�������ڸ߶�ͼ�ж�Ӧ�ĸ�����������Լ��������긳ֵ��������ǰ�Ķ���
            //iIndex++;											//�������Լ�1
        }
	}
	//printf( "%d       ", iIndex );
	m_pVertexBuffer->Unlock();

	if( FAILED( m_pd3dDevice->CreateIndexBuffer(m_iNumVertices * 6 *sizeof(WORD), 
        D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, 0 ) ) )
	{
		return FALSE;
	}
	
	//���������Լ�������
	WORD* pIndices = NULL;
    m_pIndexBuffer->Lock( 0, 0, (void**)&pIndices, 0 );
  
	m_pVertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );
	iIndex = 0;
	for(int row = 0; row < m_iCellsPerRow-1; row++)   //����ÿ��
    {
        for(int col = 0; col < m_iCellsPerCol-1; col++)  //����ÿ��
        {
			pIndices[iIndex]   =  row * m_iCellsPerRow + col;			//����A
            pIndices[iIndex+1] =  row * m_iCellsPerRow + col + 1;  //����B
            pIndices[iIndex+2] = (row+1) * m_iCellsPerRow + col;	//����C

			D3DVECTOR temp;
			ComputeTriNormal(pVertices[pIndices[iIndex]], pVertices[pIndices[iIndex+1]],pVertices[pIndices[iIndex+2]], temp);//���㷨����
			pVertices[pIndices[iIndex]]._nx = temp.x;
			pVertices[pIndices[iIndex]]._ny = temp.y;
			pVertices[pIndices[iIndex]]._nz = temp.z;

			pVertices[pIndices[iIndex+1]]._nx = temp.x;
			pVertices[pIndices[iIndex+1]]._ny = temp.y;
			pVertices[pIndices[iIndex+1]]._nz = temp.z;

			pVertices[pIndices[iIndex+2]]._nx = temp.x;
			pVertices[pIndices[iIndex+2]]._ny = temp.y;
			pVertices[pIndices[iIndex+2]]._nz = temp.z;

			pIndices[iIndex+3] = (row+1) * m_iCellsPerRow + col;		//����C
            pIndices[iIndex+4] =  row * m_iCellsPerRow + col + 1;		//����B
            pIndices[iIndex+5] = (row+1) * m_iCellsPerRow + col + 1;//����D

			ComputeTriNormal(pVertices[pIndices[iIndex+3]], pVertices[pIndices[iIndex+4]], pVertices[pIndices[iIndex+5]], temp);

			pVertices[pIndices[iIndex+3]]._nx = temp.x;
			pVertices[pIndices[iIndex+3]]._ny = temp.y;
			pVertices[pIndices[iIndex+3]]._nz = temp.z;

			pVertices[pIndices[iIndex+4]]._nx = temp.x;
			pVertices[pIndices[iIndex+4]]._ny = temp.y;
			pVertices[pIndices[iIndex+4]]._nz = temp.z;

			pVertices[pIndices[iIndex+5]]._nx = temp.x;
			pVertices[pIndices[iIndex+5]]._ny = temp.y;
			pVertices[pIndices[iIndex+5]]._nz = temp.z;

			 iIndex += 6;  //�����Լ�6
		}
	}
	m_pVertexBuffer->Unlock();
	m_pIndexBuffer->Unlock();

	return true;
}

void CTerrain::Render(IDirect3DDevice9 *Device)
{
	m_pd3dDevice->SetStreamSource( 0, m_pVertexBuffer, 0, sizeof(TERRAINVERTEX) );
	m_pd3dDevice->SetFVF( TERRAINVERTEX::FVF );
	m_pd3dDevice->SetIndices( m_pIndexBuffer );//������������  
    m_pd3dDevice->SetTexture( 0, m_pTexture );//��������
	m_pd3dDevice->SetTexture( 1, m_pTexture1 );//��������
	//m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);	//�رչ���
  
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &I);

    m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 
        m_iNumVertices, 0, m_iNumVertices * 2);		//���ƶ���
}

float CTerrain::GetHeight(int r, int l)
{
	float y;
	if(r >= 0 && l >= 0 && r < this->m_iCellsPerCol && l < m_iCellsPerRow )
	{
		y =  (float)m_vHeightInfo[r*this->m_iCellsPerRow + l];
	}
	else
	{
		y = 0;
	}
	return y;
}

void CTerrain::ComputeTriNormal(TERRAINVERTEX &v1, TERRAINVERTEX &v2, TERRAINVERTEX &v3, D3DVECTOR &normal)
{
	D3DXVECTOR3 vec1(v2._x - v1._x, v2._y - v1._y, v2._z - v1._z);
	D3DXVECTOR3 vec2(v3._x - v1._x, v3._y - v1._y, v3._z - v1._z);
	D3DXVECTOR3 temp;
	D3DXVec3Cross(&temp, &vec1, &vec2);
	D3DXVec3Normalize(&temp, &temp);
	normal = (D3DVECTOR)temp;
}