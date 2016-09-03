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
	m_fTerrainWidth = 0.0f;		//地形的宽度 
    m_fTerrainDepth = 0.0f;		//地形的深度 
    m_fCellSpacing = 0.0f;		//单元格间的间距
    m_fHeightScale = 0.0f;		//高度缩放系数 
}

CTerrain::~CTerrain(void)
{
	m_pVertexBuffer->Release();
	m_pIndexBuffer->Release();
}

float CTerrain::GetCameraHeight(float x, float z)
{
	//将地图移动到原点,方便计算
	x = ((float) m_fTerrainWidth / 2.0f) + x;
	z = ((float) m_fTerrainDepth / 2.0f) - z;
	//将地图大小单位化，方便计算
	x /= (float)m_fCellSpacing;
	z /= (float)m_fCellSpacing;
	//计算x,z坐标所在的行列值
	float col = ::floorf(x);
	float row = ::floorf(z);

	// 获取如下图4个顶点的高度
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
	if(dz < 1.0f - dx)//(x,z)点在ABC三角形上
	{
		float uy = B - A;
		float vy = C - A;

		height = A + Lerp(0.0f, uy, dx) + Lerp(0.0f, vy, dz);//线形插值得到高度
	}
	else//(x,z)点在BCD三角形上
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

	inFile.seekg( 0,ios::end );					//把文件指针移动到文件末尾

    //用模板定义一个vector<BYTE>类型的变量inData并初始化，其值为缓冲区当前位置，即缓冲区大小
	vector<BYTE> inData( inFile.tellg() );
	inFile.seekg( ios::beg );			//将文件指针移动到文件的开头，准备读取高度信息
    inFile.read( (char*)&inData[0], inData.size() );	//关键的一步，读取整个高度信息
    inFile.close();	

	m_vHeightInfo.resize(inData.size());	//将m_vHeightInfo尺寸取为缓冲区的尺寸
	for (unsigned int i=0; i<inData.size(); i++)		
        m_vHeightInfo[i] = inData[i];
	// 加载地形纹理
    if (FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, pTextureFile, &m_pTexture)))
        return FALSE;
	if (FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, pTextureFile1, &m_pTexture1)))
        return FALSE;

    return TRUE;
}

BOOL CTerrain::InitTerrain( INT iRows, INT iCols, FLOAT fSpace, FLOAT fScale )
{
	m_iCellsPerRow  = iRows;  //每行的单元格数目
    m_iCellsPerCol  = iCols;  //每列的单元格数目
    m_fCellSpacing  = fSpace;	//单元格间的间距
    m_fHeightScale  = fScale; //高度缩放系数
    m_fTerrainWidth = iRows * fSpace;  //地形的宽度
    m_fTerrainDepth = iCols * fSpace;  //地形的深度
    m_iVertsPerRow  = m_iCellsPerCol + 1;  //每行的顶点数
    m_iVertsPerCol  = m_iCellsPerRow + 1; //每列的顶点数
    m_iNumVertices  = m_iVertsPerRow * m_iVertsPerCol;  //顶点总数
	
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

	FLOAT fStartX = -m_fTerrainWidth / 2.0f, fEndX =  m_fTerrainWidth / 2.0f;		//指定起始点和结束点的X坐标值
    FLOAT fStartZ =  m_fTerrainDepth / 2.0f, fEndZ = -m_fTerrainDepth / 2.0f;	//指定起始点和结束点的Z坐标值
    FLOAT fCoordU = 80.0f / (FLOAT)m_iCellsPerRow;     //指定纹理的横坐标值
    FLOAT fCoordV = 80.0f / (FLOAT)m_iCellsPerCol;		//指定纹理的纵坐标值

	int iIndex = 0, i = 0, j = 0;
	for( float z = fStartZ; z > fEndZ; z -= m_fCellSpacing, ++i )
	{
		j = 0;
		for ( float x = fStartX; x < fEndX; x += m_fCellSpacing, ++j )	//X坐标方向上起始顶点到结束顶点行间的遍历
        {
            iIndex = i * m_iCellsPerRow + j;		//指定当前顶点在顶点缓存中的位置 
            pVertices[iIndex] = TERRAINVERTEX(x, m_vHeightInfo[iIndex], z, 0.0f, 0.0f, 0.0f, j*fCoordU, i*fCoordV, j*fCoordU, i*fCoordV ); //把顶点位置索引在高度图中对应的各个顶点参数以及纹理坐标赋值给赋给当前的顶点
            //iIndex++;											//索引数自加1
        }
	}
	//printf( "%d       ", iIndex );
	m_pVertexBuffer->Unlock();

	if( FAILED( m_pd3dDevice->CreateIndexBuffer(m_iNumVertices * 6 *sizeof(WORD), 
        D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, 0 ) ) )
	{
		return FALSE;
	}
	
	//计算索引以及法向量
	WORD* pIndices = NULL;
    m_pIndexBuffer->Lock( 0, 0, (void**)&pIndices, 0 );
  
	m_pVertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );
	iIndex = 0;
	for(int row = 0; row < m_iCellsPerRow-1; row++)   //遍历每行
    {
        for(int col = 0; col < m_iCellsPerCol-1; col++)  //遍历每列
        {
			pIndices[iIndex]   =  row * m_iCellsPerRow + col;			//顶点A
            pIndices[iIndex+1] =  row * m_iCellsPerRow + col + 1;  //顶点B
            pIndices[iIndex+2] = (row+1) * m_iCellsPerRow + col;	//顶点C

			D3DVECTOR temp;
			ComputeTriNormal(pVertices[pIndices[iIndex]], pVertices[pIndices[iIndex+1]],pVertices[pIndices[iIndex+2]], temp);//计算法向量
			pVertices[pIndices[iIndex]]._nx = temp.x;
			pVertices[pIndices[iIndex]]._ny = temp.y;
			pVertices[pIndices[iIndex]]._nz = temp.z;

			pVertices[pIndices[iIndex+1]]._nx = temp.x;
			pVertices[pIndices[iIndex+1]]._ny = temp.y;
			pVertices[pIndices[iIndex+1]]._nz = temp.z;

			pVertices[pIndices[iIndex+2]]._nx = temp.x;
			pVertices[pIndices[iIndex+2]]._ny = temp.y;
			pVertices[pIndices[iIndex+2]]._nz = temp.z;

			pIndices[iIndex+3] = (row+1) * m_iCellsPerRow + col;		//顶点C
            pIndices[iIndex+4] =  row * m_iCellsPerRow + col + 1;		//顶点B
            pIndices[iIndex+5] = (row+1) * m_iCellsPerRow + col + 1;//顶点D

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

			 iIndex += 6;  //索引自加6
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
	m_pd3dDevice->SetIndices( m_pIndexBuffer );//设置索引缓存  
    m_pd3dDevice->SetTexture( 0, m_pTexture );//设置纹理
	m_pd3dDevice->SetTexture( 1, m_pTexture1 );//设置纹理
	//m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);	//关闭光照
  
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &I);

    m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 
        m_iNumVertices, 0, m_iNumVertices * 2);		//绘制顶点
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