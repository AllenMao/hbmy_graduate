#include"CTerrain.h"

UINT log2(UINT num)
{
	UINT a = 0;
	while(num != 0)
	{
		num = num >> 1;
		a++;
	}
	return a - 1;
}

CTerrain::CTerrain(IDirect3DDevice9* pd3dDevice)
{
	m_ucpHeightData = NULL;
	
	m_pd3dDevice = pd3dDevice;

	m_pTexture = NULL;
	m_pTexture1 = NULL;

	m_tiles.textureTiles[0].LoadData("00.bmp");
	m_tiles.textureTiles[1].LoadData("01.bmp");
	m_tiles.textureTiles[2].LoadData("02.bmp");
	m_tiles.textureTiles[3].LoadData("03.bmp");
}

CTerrain::~CTerrain()
{
}

bool CTerrain::SaveHeightMap( char* szFilename )
{
	FILE* pFile;
	
	//open a file that we can write to
	pFile= fopen( szFilename, "wb" );
	if( pFile==NULL )
	{
		//could not open the ifle
		return false;
	}
	
	//check to see if we have data to actually write to a file
	if( m_ucpHeightData == NULL )
	{
		//there is no data to save
		return false;
	}
	
	//write the heightmap to a file
	fwrite( m_ucpHeightData, 1, m_iSize*m_iSize, pFile );
	
	//close the file
	fclose( pFile );
	
	return true;
}

//--------------------------------------------------------------
// Name:			CTERRAIN::NormalizeTerrain - private
// Description:		Scale the terrain height values to a range of
//					0-255
// Arguments:		-fpHeightData: the height data buffer
// Return Value:	None
//--------------------------------------------------------------
void CTerrain::NormalizeTerrain( float* fpHeightData )
{
	float fMin, fMax;
	float fHeight;
	int i;
	
	fMin= fpHeightData[0];
	fMax= fpHeightData[0];
	
	//find the min/max values of the height fTempBuffer
	for( i=1; i<m_iSize*m_iSize; i++ )
	{
		if( fpHeightData[i]>fMax ) 
			fMax= fpHeightData[i];
		
		else if( fpHeightData[i]<fMin ) 
			fMin= fpHeightData[i];
	}
	
	//find the range of the altitude
	if( fMax<=fMin )
		return;
	
	fHeight= fMax-fMin;
	
	//scale the values to a range of 0-255 (because I like things that way)
	for( i=0; i<m_iSize*m_iSize; i++ )
		fpHeightData[i]= ( ( fpHeightData[i]-fMin )/fHeight )*255.0f;
}

//--------------------------------------------------------------
// Name:			CTERRAIN::FilterHeightBand - private
// Description:		Apply the erosion filter to an individual 
//					band of height values
// Arguments:		-fpBand: the band to be filtered
//					-iStride: how far to advance per pass
//					-iCount: Number of passes to make
//					-fFilter: the filter strength
// Return Value:	None
//--------------------------------------------------------------
void CTerrain::FilterHeightBand( float* fpBand, int iStride, int iCount, float fFilter )
{
	float v= fpBand[0];
	int j  = iStride;
	int i;
	
	//go through the height band and apply the erosion filter
	for( i=0; i<iCount-1; i++ )
	{
		fpBand[j]= fFilter*v + ( 1-fFilter )*fpBand[j];
		
		v = fpBand[j];
		j+= iStride;
	}
}

//--------------------------------------------------------------
// Name:			CTERRAIN::FilterHeightfTempBuffer - private
// Description:		Apply the erosion filter to an entire buffer
//					of height values
// Arguments:		-fpHeightData: the height values to be filtered
//					-fFilter: the filter strength
// Return Value:	None
//--------------------------------------------------------------
void CTerrain::FilterHeightField( float* fpHeightData, float fFilter )
{
	int i;
	
	//erode left to right
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[m_iSize*i], 1, m_iSize, fFilter );
	
	//erode right to left
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[m_iSize*i+m_iSize-1], -1, m_iSize, fFilter );
	
	//erode top to bottom
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[i], m_iSize, m_iSize, fFilter);
	
	//erode from bottom to top
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[m_iSize*(m_iSize-1)+i], -m_iSize, m_iSize, fFilter );
}

//--------------------------------------------------------------
// Name:			CTERRAIN::MakeTerrainFault - public
// Description:		Create a height data set using the "Fault Formation"
//					algorithm.  Thanks a lot to Jason Shankel for this code!
// Arguments:		-iSize: Desired size of the height map
//					-iIterations: Number of detail passes to make
//					-iMinDelta, iMaxDelta: the desired min/max heights
//					-iIterationsPerFilter: Number of passes per filter
//					-fFilter: Strength of the filter
// Return Value:	A boolean value: -true: successful creation
//									 -false: unsuccessful creation
//--------------------------------------------------------------
bool CTerrain::MakeTerrainFault( int iSize, int iIterations, int iMinDelta, int iMaxDelta, float fFilter )
{
	float* fTempBuffer;
	int iCurrentIteration;
	int iHeight;
	int iRandX1, iRandZ1;
	int iRandX2, iRandZ2;
	int iDirX1, iDirZ1;
	int iDirX2, iDirZ2;
	int x, z;
	int i;
	
	if(m_ucpHeightData != NULL)
		delete[] m_ucpHeightData;
	//if( m_heightData.m_ucpData )
	//UnloadHeightMap( );
	
	m_iSize= iSize;
	
	//allocate the memory for our height data
	m_ucpHeightData= new unsigned char [m_iSize*m_iSize];
	fTempBuffer= new float [m_iSize*m_iSize];
	
	//check to see if memory was successfully allocated
	if( m_ucpHeightData == NULL )
	{
		//something is seriously wrong here
		
		return false;
	}
	
	//check to see if memory was successfully allocated
	if( fTempBuffer==NULL )
	{
		//something is seriously wrong here
		
		return false;
	}
	//clear the height fTempBuffer
	for( i=0; i<m_iSize*m_iSize; i++ )
		fTempBuffer[i]= 0;

	srand( (unsigned)time( NULL ) );


	for( iCurrentIteration=0; iCurrentIteration<iIterations; iCurrentIteration++ )
	{
		//calculate the height range (linear interpolation from iMaxDelta to
		//iMinDelta) for this fault-pass
		iHeight= iMaxDelta - ( ( iMaxDelta-iMinDelta )*iCurrentIteration )/iIterations;

		//pick two points at random from the entire height map
		iRandX1= rand( )%m_iSize;
		iRandZ1= rand( )%m_iSize;

		//printf("%d	%d	",iRandX1,iRandZ1);
		
		//check to make sure that the points are not the same
		do
		{
			iRandX2= rand( )%m_iSize;
			iRandZ2= rand( )%m_iSize;

			//printf("%d	%d\n",iRandX2,iRandZ2);

		} while ( iRandX2==iRandX1 && iRandZ2==iRandZ1 );
		
		
		//iDirX1, iDirZ1 is a vector going the same direction as the line
		iDirX1= iRandX2-iRandX1;
		iDirZ1= iRandZ2-iRandZ1;
		
		for( z=0; z<m_iSize; z++ )
		{
			for( x=0; x<m_iSize; x++ )
			{
				//iDirX2, iDirZ2 is a vector from iRandX1, iRandZ1 to the current point (in the loop)
				iDirX2= x-iRandX1;
				iDirZ2= z-iRandZ1;
				
				//if the result of ( iDirX2*iDirZ1 - iDirX1*iDirZ2 ) is "up" (above 0), 
				//then raise this point by iHeight
				if( ( iDirX2*iDirZ1 - iDirX1*iDirZ2 )>0 )
					fTempBuffer[( z*m_iSize )+x]+= ( float )iHeight;
			}
		}
		
		//erode terrain
		FilterHeightField( fTempBuffer, fFilter );
	}
	
	//normalize the terrain for our purposes
	NormalizeTerrain( fTempBuffer );
	
	//transfer the terrain into our class's unsigned char height buffer
	for( z=0; z<m_iSize; z++ )
	{
		for( x=0; x<m_iSize; x++ )
			m_ucpHeightData[( z*m_iSize )+x]= ( unsigned char )fTempBuffer[( z*m_iSize )+x];
	}
	
	//delete temporary buffer
	if( fTempBuffer )
	{
		//delete the data
		delete[] fTempBuffer;
	}
	
	return true;
}

//--------------------------------------------------------------
// Name:			CTERRAIN::RegionPercent - public
// Description:		Get the percentage of which a texture tile should be
//					visible at a given height
// Arguments:		-tileType: type of tile to check
//					-ucHeight: the current height to test for
// Return Value:	A floating point value: the percentage of which the
//					current texture occupies at the given height
//--------------------------------------------------------------
float CTerrain::RegionPercent( int tileType, unsigned char ucHeight )
{
	float fTemp1, fTemp2;

	//if the height is lower than the lowest tile's height, then we want full brightness,
	//if we don't do this, the area will get darkened, and no texture will get shown
	if( m_tiles.textureTiles[LOWEST_TILE].IsLoaded( ) )
	{
		if( tileType==LOWEST_TILE && ucHeight<m_tiles.m_regions[LOWEST_TILE].m_iOptimalHeight )
			return 1.0f;
	}
	else if( m_tiles.textureTiles[LOW_TILE].IsLoaded( ) )
	{
		if( tileType==LOW_TILE && ucHeight<m_tiles.m_regions[LOW_TILE].m_iOptimalHeight )
			return 1.0f;
	}
	else if( m_tiles.textureTiles[HIGH_TILE].IsLoaded( ) )
	{
		if( tileType==HIGH_TILE && ucHeight<m_tiles.m_regions[HIGH_TILE].m_iOptimalHeight )
			return 1.0f;
	}
	else if( m_tiles.textureTiles[HIGHEST_TILE].IsLoaded( ) )
	{
		if( tileType==HIGHEST_TILE && ucHeight<m_tiles.m_regions[HIGHEST_TILE].m_iOptimalHeight )
			return 1.0f;
	}

	//height is lower than the region's boundary
	if( ucHeight<m_tiles.m_regions[tileType].m_iLowHeight )
		return 0.0f;

	//height is higher than the region's boundary
	else if( ucHeight>m_tiles.m_regions[tileType].m_iHighHeight )
		return 0.0f;

	//height is below the optimum height
	if( ucHeight<m_tiles.m_regions[tileType].m_iOptimalHeight )
	{
		//calculate the texture percentage for the given tile's region
		fTemp1= ( float )ucHeight-m_tiles.m_regions[tileType].m_iLowHeight;
		fTemp2= ( float )m_tiles.m_regions[tileType].m_iOptimalHeight-m_tiles.m_regions[tileType].m_iLowHeight;

		return ( fTemp1/fTemp2 );
	}

	//height is exactly the same as the optimal height
	else if( ucHeight==m_tiles.m_regions[tileType].m_iOptimalHeight )
		return 1.0f;

	//height is above the optimal height
	else if( ucHeight>m_tiles.m_regions[tileType].m_iOptimalHeight )
	{
		//calculate the texture percentage for the given tile's region
		fTemp1= ( float )m_tiles.m_regions[tileType].m_iHighHeight-m_tiles.m_regions[tileType].m_iOptimalHeight;

		return ( ( fTemp1-( ucHeight-m_tiles.m_regions[tileType].m_iOptimalHeight ) )/fTemp1 );
	}

	//something is seriously wrong if the height doesn't fit the previous cases
	return 0.0f;
}

//--------------------------------------------------------------
// Name:			CTERRAIN::GetTexCoords - public
// Description:		Get texture coordinates :)
//					present in the final texture
// Arguments:		-texture: the texture to get coordinates for
//					-*x, *y: the unaltered texture coordinates, and the
//							 storage place for the altered coordinates
// Return Value:	None
//--------------------------------------------------------------
void CTerrain::GetTexCoords( CImage texture, unsigned int* x, unsigned int* y )
{
	unsigned int uiWidth = texture.GetWidth( );
	unsigned int uiHeight= texture.GetHeight( );
	int iRepeatX= -1;
	int iRepeatY= -1;
	int i= 0;

	//loop until we figure out how many times the tile has repeated (on the X axis)
	while( iRepeatX==-1 )
	{
		i++;

		//if x is less than the total width, then we found a winner!
		if( *x<( uiWidth*i ) )
			iRepeatX= i-1;
	}

	//prepare to figure out the repetition on the Y axis
	i= 0;

	//loop until we figure out how many times the tile has repeated (on the Y axis)
	while( iRepeatY==-1 )
	{
		i++;

		//if y is less than the total height, then we have a bingo!
		if( *y<( uiHeight*i ) )
			iRepeatY= i-1;
	}

	//update the given texture coordinates
	*x= *x-( uiWidth*iRepeatX );
	*y= *y-( uiHeight*iRepeatY );
}

//--------------------------------------------------------------
// Name:			CTERRAIN::InterpolateHeight - public
// Description:		Interpolate the heights in the height map so that
//					the generated texture map does not look incredibly blocky
// Arguments:		-x, z: coordinates to get the height at
//					-fHeightToTexRatio: Height map size to texture 
//										map size ratio
// Return Value:	An unsigned char value: the interpolated height
//--------------------------------------------------------------
unsigned char CTerrain::InterpolateHeight( int x, int z, float fHeightToTexRatio )
{
	unsigned char ucLow, ucHighX, ucHighZ;
	float ucX, ucZ;
	float fScaledX= x*fHeightToTexRatio;
	float fScaledZ= z*fHeightToTexRatio;
	float fInterpolation;

	//set the middle boundary
	ucLow= GetTrueHeightAtPoint( ( int )fScaledX, ( int )fScaledZ );

	//start off by interpolating along the X axis
	//set the high boundary
	if( ( fScaledX+1 )>m_iSize )
		return ucLow;
	else
		ucHighX= GetTrueHeightAtPoint( ( int )fScaledX+1, ( int )fScaledZ );

	//calculate the interpolation (for the X axis)
	fInterpolation= ( fScaledX-( int )fScaledX );
	ucX= ( ( ucHighX-ucLow )*fInterpolation )+ucLow;

	//interpolate along the Z axis now
	//set the high boundary
	if( ( fScaledZ+1 )>m_iSize )
		return ucLow;
	else
		ucHighZ= GetTrueHeightAtPoint( ( int )fScaledX, ( int )fScaledZ+1 );

	//calculate the interpolation (for the Z axis)
	fInterpolation= ( fScaledZ-( int )fScaledZ );
	ucZ= ( ( ucHighZ-ucLow )*fInterpolation )+ucLow;

	//calculate the overall interpolation (average of the two values)
	return ( ( unsigned char )( ( ucX+ucZ )/2 ) );
}

//--------------------------------------------------------------
// Name:			CTERRAIN::GenerateTextureMap - public
// Description:		Generate a texture map from the four tiles (that must
//					be loaded before this function is called)
// Arguments:		-uiSize: the size of the texture map to be generated
// Return Value:	None
//--------------------------------------------------------------
void CTerrain::GenerateTextureMap( unsigned int uiSize )
{
	unsigned char ucRed, ucGreen, ucBlue;
	unsigned int x, z;
	unsigned int uiTexX, uiTexZ;
	float fTotalRed, fTotalGreen, fTotalBlue;
	float fBlend[4];
	float fMapRatio;
	int iLastHeight;
	int i;

	//find out the number of tiles that we have
	m_tiles.iNumTiles= 0;
	for( i=0; i<TRN_NUM_TILES; i++ )
	{

		//if the current tile is loaded, then we add one to the total tile count
		if( m_tiles.textureTiles[i].IsLoaded( ) )
			m_tiles.iNumTiles++;
	}

	//now, re-loop through, and calculate the texture regions
	iLastHeight= -1;
	for( i=0; i<TRN_NUM_TILES; i++ )
	{
		//we only want to perform these calculations if we actually have a tile loaded
		if( m_tiles.textureTiles[i].IsLoaded( ) )
		{
			//calculate the three height boundaries
			m_tiles.m_regions[i].m_iLowHeight= iLastHeight+1;
			iLastHeight+= 255/m_tiles.iNumTiles;

			m_tiles.m_regions[i].m_iOptimalHeight= iLastHeight;

			m_tiles.m_regions[i].m_iHighHeight= ( iLastHeight-m_tiles.m_regions[i].m_iLowHeight )+iLastHeight;
		}
	}

	//create room for a new texture
	m_texture.Create( uiSize, uiSize, 24 );

	//get the height map to texture map ratio (since, most of the time,
	//the texture map will be a higher resolution than the height map, so
	//we need the ratio of height map pixels to texture map pixels)
	fMapRatio= ( float )m_iSize/uiSize;

	//time to create the texture data
	for( z=0; z<uiSize; z++ )
	{
		for( x=0; x<uiSize; x++ )
		{
			//set our total color counters to 0.0f
			fTotalRed  = 0.0f;
			fTotalGreen= 0.0f;
			fTotalBlue = 0.0f;

			//loop through the tiles (for the third time in this function!)
			for( i=0; i<TRN_NUM_TILES; i++ )
			{
				//if the tile is loaded, we can do the calculations
				if( m_tiles.textureTiles[i].IsLoaded( ) )
				{
					uiTexX= x;
					uiTexZ= z;

					//get texture coordinates
					GetTexCoords( m_tiles.textureTiles[i], &uiTexX, &uiTexZ );

					//get the current color in the texture at the coordinates that we got
					//in GetTexCoords
					m_tiles.textureTiles[i].GetColor( uiTexX, uiTexZ, &ucRed, &ucGreen, &ucBlue );

					//get the current coordinate's blending percentage for this tile
					fBlend[i]= RegionPercent( i, InterpolateHeight( x, z, fMapRatio ) );

					//calculate the RGB values that will be used
					fTotalRed  += ucRed*fBlend[i];
					fTotalGreen+= ucGreen*fBlend[i];
					fTotalBlue += ucBlue*fBlend[i];
				}
			}			

			//set our terrain's texture color to the one that we previously calculated
			m_texture.SetColor( x, z, ( unsigned char )fTotalRed,
									  ( unsigned char )fTotalGreen,
									  ( unsigned char )fTotalBlue );
		}
	}

	m_texture.SaveBMP("Finish.bmp");
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CTerrain::LoadTerrainFromFile( char* pRawFileName, char* pTextureFile, char* pTextureFile1 )
{
	//////////////////LOD//////////////////////////////
	m_pHeadNode = NULL;
	m_numNodeRendered = 0;
	// 从文件中读取地形数据
	ifstream inFile;
	inFile.open( pRawFileName, ios::binary );
	
	inFile.seekg( 0,ios::end );					//把文件指针移动到文件末尾
	
	//用模板定义一个vector<BYTE>类型的变量inData并初始化，其值为缓冲区当前位置，即缓冲区大小
	std::vector<BYTE> inData( inFile.tellg() );
	inFile.seekg( ios::beg );			//将文件指针移动到文件的开头，准备读取高度信息
	inFile.read( (char*)&inData[0], inData.size() );	//关键的一步，读取整个高度信息
	inFile.close();

	m_iSize = m_width = (int)sqrt(inData.size()*1.0f);

	//freopen("01.txt","w",stdout);
	//printf("%d\n",m_width);
	//puts("sb");
	// 开辟二维高程数组,并以读取数据将其初始化
	m_elevArray = new float*[m_width];
	UINT i,j;
	for(i=0;i<m_width;i++)
	{
		m_elevArray[i] = new float[m_width];
		for(j=0;j<m_width;j++)
			m_elevArray[i][j] = inData[i * m_width + j];
	}


	// 开辟纹理数组
	m_tuArray = new float[m_width];
	m_tvArray = new float[m_width];

	//在SetDimesion(...)函数中开辟空间，只有知道网格的几何宽度才能求取法线向量的值
	m_normalArray = NULL;

	for(i=0;i<m_width;i++)
	{
		m_tvArray[i] = m_tuArray[i] = i * 1.0f/ (int)(m_width-1);
	}

	m_pVB = NULL;
	m_pIB = NULL;
	m_vertexArray = NULL;
	m_indexArray = NULL;

	/////////////////////////////////////////////////////////////////////////////
	
	if(m_ucpHeightData != NULL)
	{
		delete[] m_ucpHeightData;
		m_ucpHeightData = NULL;
	}
	m_ucpHeightData = new unsigned char[inData.size()];

	for (i=0; i<inData.size(); i++)		
         m_ucpHeightData[i] = inData[i];

	// 加载地形纹理
	if(pTextureFile == NULL) return TRUE;

	// 创建纹理
	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, pTextureFile, &m_pTexture )))
	{
		char strTexFileName[60];
		sprintf( strTexFileName, "..\\%s", m_pTexture );
		if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, pTextureFile, &m_pTexture )))
			m_pTexture = NULL;
	}
	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, pTextureFile1, &m_pTexture1 )))
	{
		char strTexFileName[60];
		sprintf( strTexFileName, "..\\%s", m_pTexture1 );
		if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, pTextureFile1, &m_pTexture1 )))
			m_pTexture1 = NULL;
	}
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//节点类
CLODTrnNode::CLODTrnNode()
{
	m_pTLChildNode = NULL;
	m_pBLChildNode = NULL;
	m_pTRChildNode = NULL;
	m_pBRChildNode = NULL;
}

CLODTrnNode::~CLODTrnNode()
{
	SAFE_DELETE( m_pTLChildNode );
	SAFE_DELETE( m_pBLChildNode );
	SAFE_DELETE( m_pTRChildNode );
	SAFE_DELETE( m_pBRChildNode );
}

CLODTrnNode::CLODTrnNode(NODEPOSSIZE nodePosSize)
{
	m_pParentNode = NULL;
	m_bIsLeaf = true;
	m_bReachAcceptableLevel = false;
	m_bTouched = false;
	m_bIsVisible = false;

	m_pTLChildNode = NULL;
	m_pBLChildNode = NULL;
	m_pTRChildNode = NULL;
	m_pBRChildNode = NULL;

	m_nodePosSize = nodePosSize;
}

void CLODTrnNode::SplitNode()
{
	UINT sizeDivide2 = m_nodePosSize.size >> 1;
	UINT sizeDivide4 = sizeDivide2 >> 1;
	// 左上角子节点
	NODEPOSSIZE nodePosSize( m_nodePosSize.x - sizeDivide4, m_nodePosSize.y - sizeDivide4, sizeDivide2 );
	m_pTLChildNode = new CLODTrnNode( nodePosSize );
	m_pTLChildNode->m_pParentNode = this;
	m_pTLChildNode->m_level = m_level - 1;
	m_pTLChildNode->m_EnumValue = TOPLEFT;

	// 左下角子节点
	nodePosSize.y += sizeDivide2;
	m_pBLChildNode = new CLODTrnNode( nodePosSize );
	m_pBLChildNode->m_pParentNode = this;
	m_pBLChildNode->m_level = m_level -1;
	m_pBLChildNode->m_EnumValue = BOTTOMLEFT;

	// 右下角子节点
	nodePosSize.x += sizeDivide2;
	m_pBRChildNode = new CLODTrnNode( nodePosSize );
	m_pBRChildNode->m_pParentNode = this;
	m_pBRChildNode->m_level = m_level - 1;
	m_pBRChildNode->m_EnumValue = BOTTOMRIGHT;

	// 右上角子节点
	nodePosSize.y -= sizeDivide2;
	m_pTRChildNode = new CLODTrnNode( nodePosSize );
	m_pTRChildNode->m_pParentNode = this;
	m_pTRChildNode->m_level = m_level -1;
	m_pTRChildNode->m_EnumValue = TOPRIGHT;
	
	// 分裂节点后，便不再是叶子节点
	m_bIsLeaf = false;
}

BOOL CLODTrnNode::IsSplit()
{
	if( m_level > 0 )
		return TRUE;
	else
		return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////////
HRESULT CTerrain::InitShader()
{
	HRESULT hr;
	LPD3DXBUFFER pShaderBuffer;
	// Create vertex shader
	if( FAILED( hr = D3DXCompileShaderFromFile( "hlsl.vsd", NULL, NULL, "main", 
		"vs_2_0", 0/*D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION*//*Flag*/, &pShaderBuffer, NULL/*Error buffer*/, &m_pVConstTable )))
		return hr;
	if( FAILED( hr = m_pd3dDevice->CreateVertexShader((DWORD*)pShaderBuffer->GetBufferPointer(), &m_pVShader)))
		return hr;
	// Create pixel shader
	if( FAILED( hr = D3DXCompileShaderFromFile( "hlsl.psd", NULL, NULL, "main", 
		"ps_2_0", 0/*Flag*/, &pShaderBuffer, NULL/*Error buffer*/, &m_pPConstTable )))
		return hr;
	if( FAILED( hr = m_pd3dDevice->CreatePixelShader((DWORD*)pShaderBuffer->GetBufferPointer(), &m_pPShader)))
		return hr;
	return hr;
}

void CTerrain::InitQuadTree()
{
	m_pHeadNode = new CLODTrnNode( NODEPOSSIZE( (m_width-1) >> 1, (m_width-1) >> 1, m_width-1 ) );
	m_pHeadNode->m_level = log2( m_width - 1 ) - 1;
	GrowQuadTree(m_pHeadNode);
}

void CTerrain::GrowQuadTree(CLODTrnNode* pNode)
{
	if(pNode->IsSplit())
	{
		pNode->SplitNode();
		GrowQuadTree( pNode->m_pBLChildNode );
		GrowQuadTree( pNode->m_pTLChildNode );
		GrowQuadTree( pNode->m_pTRChildNode );
		GrowQuadTree( pNode->m_pBRChildNode );
	}
}

void CTerrain::SetDimesion( float width, float height,D3DXVECTOR2 centerPt/*地形的中点位置*/ )
{
	m_fWidth = width;
	m_fHeight = height;
	m_centerPt = centerPt;

	m_GridWidth = m_fWidth/(m_width - 1);
	m_GridHeight = m_fHeight/(m_width - 1);

	// 开辟二维法线向量数组,并初始化
	UINT i,j;

	m_normalArray = new D3DXVECTOR3*[m_width];
	for(i=0;i<m_width;i++)
	{
		m_normalArray[i] = new D3DXVECTOR3[m_width];
	}
	// 二维法线向量数组初始化，不包括边界部分
	for(i=0;i<m_width - 1;i++)
	{
		for(j=0;j<m_width - 1;j++)
		{
			D3DXVECTOR3 vec[2];
			vec[0] = D3DXVECTOR3( -m_GridWidth, m_elevArray[i][j] - m_elevArray[i+1][j], 0 );
			vec[1] = D3DXVECTOR3( 0, m_elevArray[i][j] - m_elevArray[i][j+1], m_GridHeight );
			D3DXVec3Cross( &m_normalArray[i][j], &vec[0], &vec[1] );
			D3DXVec3Normalize( &m_normalArray[i][j], &m_normalArray[i][j] );
		}
	}
	// 地形边界处 法线向量的初始化
	for(i=0;i<m_width;i++)
	{
		m_normalArray[i][m_width-1] = D3DXVECTOR3(0, 1, 0);
		m_normalArray[m_width-1][i] = D3DXVECTOR3(0, 1, 0);
	}
}
void CTerrain::CreateTerrain(CCamera* pCamera )
{
	
	m_pCamera = pCamera;
	// 将根节点及其所有子节点的属性还原为false
	ClearTreeFlag(m_pHeadNode);
	
	// 遍历四叉树，判断节点的可见性和它的level是否达到要求，并对节点进行标记
	m_numNodeRendered = 0;
	m_frustum.Construct( m_pd3dDevice );
	
	ClimbQuadTree(m_pHeadNode, m_pd3dDevice);
	// 动态开辟顶点数组和索引数组
	UINT vertexArraySize = (m_numNodeRendered<<3) + m_numNodeRendered;  //m_numNodeRendered * 9
	UINT indexArraySize  = (m_numNodeRendered<<4) + (m_numNodeRendered<<3);   //m_numNodeRendered * 24
	m_vertexArray = new TRNVERTEX[vertexArraySize];
	m_indexArray  = new WORD[indexArraySize];
	// 遍历四叉树，寻找树中m_bReachAcceptableLevel == true的节点，并将所有符合条件的节点写进顶点数组和索引数组中
	m_vertexIndex = 0;
	m_indexIndex  = 0;

	RefineNode(m_pHeadNode);

	// 创建顶点缓冲和索引缓冲,还需设置某些flag以提高性能
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);

	// 创建 vertex buffer
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( vertexArraySize * sizeof(TRNVERTEX),
        D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY/*Usage*/, D3DFVF_TRNVERTEX, D3DPOOL_SYSTEMMEM/*D3DPOOL*/, &m_pVB, NULL )))
		return;
	void* pVertices;
	if( FAILED(m_pVB->Lock( 0/*offset*/, vertexArraySize * sizeof(TRNVERTEX), (void**)&pVertices, 
		D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE/*Flag*/ )))
		return;
	memcpy( pVertices, m_vertexArray, vertexArraySize * sizeof(TRNVERTEX) );
	m_pVB->Unlock();

	// 创建 index buffer
	if( FAILED( m_pd3dDevice->CreateIndexBuffer( indexArraySize *sizeof(WORD), 
		D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY/*Usage*/, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM/*D3DPOOL*/, &m_pIB, NULL )))
		return;
	void* pIndices;
	if( FAILED(m_pIB->Lock( 0/*offset*/, indexArraySize * sizeof(WORD), (void**)&pIndices,
		D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE/*Flag*/ )))
		return;
	memcpy( pIndices, m_indexArray, indexArraySize * sizeof(WORD) );
	m_pIB->Unlock();

	// 创建 vertex buffer完毕，释放顶点数组
	delete [] m_vertexArray;
	// 创建 index buffer完毕，释放索引数组
	delete [] m_indexArray;
}

// 遍历四叉树，判断节点的可见性和它的level是否达到要求，并对节点进行标记
void CTerrain::ClimbQuadTree( CLODTrnNode* pNode, LPDIRECT3DDEVICE9 pd3dDevice )
{
	pNode->m_bTouched = true;
	if( IsVisible( pNode, pd3dDevice ) )// 如果头结点可见，对该节点进行处理，否则，忽略之
	{
		pNode->m_bIsVisible = true;
		if( IsReachAcceptableLevel( pNode )||pNode->m_bIsLeaf )
		{// 如果该节点的level已足够低，或者该节点是叶子节点，不再往下遍历
			pNode->m_bReachAcceptableLevel = true;
			// 下面这句话要求每个节点只能被扩展一次
			m_numNodeRendered ++;
		}
		else
		{// 如果该节点的level还不够低，并且不是叶子节点，继续往下扩展遍历
			// 往下扩展遍历之前，确认它的四个相邻顶点的父节点被扩展遍历过
			pNode->m_bReachAcceptableLevel = false;
			CLODTrnNode* pAdjNodeArray[4];
			GetAdjNodeParent4( pAdjNodeArray, pNode );
			for(int i=0;i<4;i++)
			{
				if( pAdjNodeArray[i] != NULL && pAdjNodeArray[i]->m_bTouched == false )
					ClimbQuadTree( pAdjNodeArray[i], pd3dDevice );
			}
			if( false == pNode->m_pBLChildNode->m_bTouched )
				ClimbQuadTree( pNode->m_pBLChildNode, pd3dDevice );
			if( false == pNode->m_pTLChildNode->m_bTouched )
				ClimbQuadTree( pNode->m_pTLChildNode, pd3dDevice );
			if( false == pNode->m_pBRChildNode->m_bTouched )
				ClimbQuadTree( pNode->m_pBRChildNode, pd3dDevice );
			if( false == pNode->m_pTRChildNode->m_bTouched )
				ClimbQuadTree( pNode->m_pTRChildNode, pd3dDevice );
		}
	}
	else
	{
		pNode->m_bIsVisible = false;
		pNode->m_bReachAcceptableLevel = false;
	}
}

// 判断某个节点是否可见
BOOL CTerrain::IsVisible(CLODTrnNode* pNode, LPDIRECT3DDEVICE9 pd3dDevice)
{
	NODEPOSSIZE posSize = pNode->GetPosSize();
	D3DXVECTOR3 centerPos;
	centerPos.x = ( int(posSize.x - ((m_width - 1)>>1)) ) * m_GridWidth + m_centerPt.x;
	centerPos.y = m_elevArray[posSize.y][posSize.x];
	centerPos.z = ( int(posSize.y - ((m_width -1 )>>1)) ) * m_GridHeight + m_centerPt.y;
	float radius = float((int)(posSize.size) * m_GridWidth)/1.414f;
	return m_frustum.CheckSphere( centerPos, radius );
}

// 判断某个节点的level是否已足够低
BOOL CTerrain::IsReachAcceptableLevel(CLODTrnNode* pNode)
{
	// 先求取节点中心的绝对坐标值
	NODEPOSSIZE posSize = pNode->GetPosSize();
	D3DXVECTOR3 centerPos;
	centerPos.x = ( int(posSize.x - ((m_width - 1)>>1)) ) * m_GridWidth + m_centerPt.x;
	centerPos.y = m_elevArray[posSize.y][posSize.x];
	centerPos.z = ( int(posSize.y - ((m_width -1 )>>1)) ) * m_GridHeight + m_centerPt.y;
	// 再获取眼睛的绝对坐标值
	D3DXVECTOR3 eyePt( m_pCamera->GetPosX(), m_pCamera->GetPosY(), m_pCamera->GetPosZ() );
	// 求取眼睛与节点中心之间的绝对距离
	D3DXVECTOR3 vec3 = centerPos - eyePt;
	float distance = D3DXVec3Length( &vec3 );
	////
	D3DXVECTOR3 lookAtVec;
	m_pCamera->GetLookVec(&lookAtVec);
	D3DXVec3Normalize(&lookAtVec, &lookAtVec);
	D3DXVec3Normalize(&lookAtVec, &lookAtVec);
	D3DXVec3Normalize(&vec3, &vec3);
	float fNum = D3DXVec3Dot(&vec3, &lookAtVec);
	if(fNum < 0)
		fNum = 0;
	fNum = fNum/2 + 0.5f;
	if( distance / fNum > float(posSize.size << 4) )
		return TRUE;
	else
		return FALSE;
}

// 得到节点的四个相邻节点的父节点，将它们的指针存储在一个数组里
void CTerrain::GetAdjNodeParent4(CLODTrnNode** pAdjNodeArray, CLODTrnNode* pNode)
{
	NODEPOSSIZE posSize = pNode->GetPosSize();
	// 查找该节点上面的那个节点的父节点
	pAdjNodeArray[0] = FindParentNode( posSize.x, posSize.y - posSize.size );
	// 查找该节点下面的那个节点的父节点
	pAdjNodeArray[1] = FindParentNode( posSize.x, posSize.y + posSize.size );
	// 查找该节点左边的那个节点的父节点
	pAdjNodeArray[2] = FindParentNode( posSize.x - posSize.size, posSize.y );
	// 查找该节点右边的那个节点的父节点
	pAdjNodeArray[3] = FindParentNode( posSize.x + posSize.size, posSize.y );
}

// 查找相对位置为(x, y)的那个节点的父节点
CLODTrnNode* CTerrain::FindParentNode(UINT x, UINT y)
{
	CLODTrnNode* pNode = m_pHeadNode;
	while(1)
	{
		if( pNode == NULL )
			return NULL;
		NODEPOSSIZE posSize = pNode->GetPosSize();
		if( posSize.x == x )
			return pNode->m_pParentNode; // 找到该节点，返回它的父节点
		else if( posSize.x < x )
		{
			if( posSize.y < y )
				pNode = pNode->m_pBRChildNode;
			else //posSize.y > y
				pNode = pNode->m_pTRChildNode;
		}
		else //posSize.x > x
		{
			if( posSize.y < y )
				pNode = pNode->m_pBLChildNode;
			else //posSize.y > y
				pNode = pNode->m_pTLChildNode;
		}
	}
}

// 遍历四叉树，寻找树中m_bReachAcceptableLevel == true的节点，并将所有符合条件的节点写进顶点数组和索引数组中
// 每个节点由8个三角形组成，9个顶点数据，24个索引数据
void CTerrain::RefineNode(CLODTrnNode* pNode)
{
	if( pNode->m_bIsVisible )
	{
		if( pNode->m_bReachAcceptableLevel)/* || !pNode->m_bIsVisible*//*for debug*/
		{// 节点的细节已足够，将它送入顶点数组和索引数组
			// 顶点数组赋值，   0 1 2
			//                  3 4 5 
			//                  6 7 8
			// 计算顶点的位置向量和纹理坐标
			NODEPOSSIZE posSize = pNode->GetPosSize();
			UINT sizeDivide2 = posSize.size >> 1;
			float top = ( int(posSize.y - sizeDivide2 - ((m_width -1 )>>1)) ) * m_GridHeight + m_centerPt.y;
			float bottom = ( int(posSize.y + sizeDivide2 - ((m_width -1 )>>1)) ) * m_GridHeight + m_centerPt.y;
			float left = ( int(posSize.x - sizeDivide2 - ((m_width -1 )>>1)) ) * m_GridWidth + m_centerPt.x;
			float right = ( int(posSize.x + sizeDivide2 - ((m_width -1 )>>1)) ) * m_GridWidth + m_centerPt.x;
			float centerX = ( int(posSize.x - ((m_width -1 )>>1)) ) * m_GridWidth + m_centerPt.x;
			float centerY = ( int(posSize.y - ((m_width -1 )>>1)) ) * m_GridWidth + m_centerPt.y;

			// 左上角顶点
			m_vertexArray[m_vertexIndex + 0].position.x = left;
			m_vertexArray[m_vertexIndex + 0].position.z = top;
			m_vertexArray[m_vertexIndex + 0].position.y = m_elevArray[posSize.y - sizeDivide2][posSize.x - sizeDivide2];
			m_vertexArray[m_vertexIndex + 0].normal   = m_normalArray[posSize.y - sizeDivide2][posSize.x - sizeDivide2];
			m_vertexArray[m_vertexIndex + 0].tu = m_tuArray[posSize.x - sizeDivide2];
			m_vertexArray[m_vertexIndex + 0].tv = m_tvArray[posSize.y - sizeDivide2];

			// 上边顶点
			// 判断它上方的那个相邻节点的 m_bTouched 属性是否为true,如为真从数组中读取高程值，否则取相邻顶点高程值的平均值
			m_vertexArray[m_vertexIndex + 1].position.x = centerX;
			m_vertexArray[m_vertexIndex + 1].position.z = top;
			CLODTrnNode* pAdjNode = FindNode( posSize.x, posSize.y - posSize.size );
			if( pAdjNode != NULL )
			{
				if( pAdjNode->m_bTouched )
				{
					m_vertexArray[m_vertexIndex + 1].position.y = m_elevArray[posSize.y - sizeDivide2][posSize.x];
					m_vertexArray[m_vertexIndex + 1].normal   = m_normalArray[posSize.y - sizeDivide2][posSize.x];
				}
				else
				{
					m_vertexArray[m_vertexIndex + 1].position.y = ( m_elevArray[posSize.y - sizeDivide2][posSize.x - sizeDivide2]
						+ m_elevArray[posSize.y - sizeDivide2][posSize.x + sizeDivide2])/2;
					m_vertexArray[m_vertexIndex + 1].normal = ( m_normalArray[posSize.y - sizeDivide2][posSize.x - sizeDivide2]
						+ m_normalArray[posSize.y - sizeDivide2][posSize.x + sizeDivide2])/2;
				}
			}
			else
			{
				m_vertexArray[m_vertexIndex + 1].position.y = m_elevArray[posSize.y - sizeDivide2][posSize.x];
				m_vertexArray[m_vertexIndex + 1].normal   = m_normalArray[posSize.y - sizeDivide2][posSize.x];
			}
			m_vertexArray[m_vertexIndex + 1].tu = m_tuArray[posSize.x];
			m_vertexArray[m_vertexIndex + 1].tv = m_tvArray[posSize.y - sizeDivide2];

			// 右上角顶点
			m_vertexArray[m_vertexIndex + 2].position.x = right;
			m_vertexArray[m_vertexIndex + 2].position.z = top;
			m_vertexArray[m_vertexIndex + 2].position.y = m_elevArray[posSize.y - sizeDivide2][posSize.x + sizeDivide2];
			m_vertexArray[m_vertexIndex + 2].normal   = m_normalArray[posSize.y - sizeDivide2][posSize.x + sizeDivide2];
			m_vertexArray[m_vertexIndex + 2].tu = m_tuArray[posSize.x + sizeDivide2];
			m_vertexArray[m_vertexIndex + 2].tv = m_tvArray[posSize.y - sizeDivide2];

			// 左边顶点
			// 判断它左方的那个相邻节点的 m_bTouched 属性是否为true,如为真从数组中读取高程值，否则取相邻顶点高程值的平均值
			m_vertexArray[m_vertexIndex + 3].position.x = left;
			m_vertexArray[m_vertexIndex + 3].position.z = centerY;
			pAdjNode = FindNode( posSize.x - posSize.size, posSize.y );
			if( pAdjNode != NULL )
			{
				if( pAdjNode->m_bTouched )
				{
					m_vertexArray[m_vertexIndex + 3].position.y = m_elevArray[posSize.y][posSize.x - sizeDivide2];
					m_vertexArray[m_vertexIndex + 3].normal   = m_normalArray[posSize.y][posSize.x - sizeDivide2];
				}
				else
				{
					m_vertexArray[m_vertexIndex + 3].position.y = ( m_elevArray[posSize.y - sizeDivide2][posSize.x - sizeDivide2]
						+ m_elevArray[posSize.y + sizeDivide2][posSize.x - sizeDivide2])/2;
					m_vertexArray[m_vertexIndex + 3].normal = ( m_normalArray[posSize.y - sizeDivide2][posSize.x - sizeDivide2]
						+ m_normalArray[posSize.y + sizeDivide2][posSize.x - sizeDivide2])/2;
				}
			}
			else
			{
				m_vertexArray[m_vertexIndex + 3].position.y = m_elevArray[posSize.y][posSize.x - sizeDivide2];
				m_vertexArray[m_vertexIndex + 3].normal   = m_normalArray[posSize.y][posSize.x - sizeDivide2];
			}
			m_vertexArray[m_vertexIndex + 3].tu = m_tuArray[posSize.x - sizeDivide2];
			m_vertexArray[m_vertexIndex + 3].tv = m_tvArray[posSize.y];

			// 中心顶点
			m_vertexArray[m_vertexIndex + 4].position.x = centerX;
			m_vertexArray[m_vertexIndex + 4].position.z = centerY;
			m_vertexArray[m_vertexIndex + 4].position.y = m_elevArray[posSize.y][posSize.x];
			m_vertexArray[m_vertexIndex + 4].normal   = m_normalArray[posSize.y][posSize.x];
			m_vertexArray[m_vertexIndex + 4].tu = m_tuArray[posSize.x];
			m_vertexArray[m_vertexIndex + 4].tv = m_tvArray[posSize.y];

			// 右边顶点
			// 判断它左方的那个相邻节点的 m_bTouched 属性是否为true,如为真从数组中读取高程值，否则取相邻顶点高程值的平均值
			m_vertexArray[m_vertexIndex + 5].position.x = right;
			m_vertexArray[m_vertexIndex + 5].position.z = centerY;
			pAdjNode = FindNode( posSize.x + posSize.size, posSize.y );
			if( pAdjNode != NULL )
			{
				if( pAdjNode->m_bTouched )
				{
					m_vertexArray[m_vertexIndex + 5].position.y = m_elevArray[posSize.y][posSize.x + sizeDivide2];
					m_vertexArray[m_vertexIndex + 5].normal   = m_normalArray[posSize.y][posSize.x + sizeDivide2];
				}
				else
				{
					m_vertexArray[m_vertexIndex + 5].position.y = ( m_elevArray[posSize.y - sizeDivide2][posSize.x + sizeDivide2]
						+ m_elevArray[posSize.y + sizeDivide2][posSize.x + sizeDivide2])/2;
					m_vertexArray[m_vertexIndex + 5].normal = ( m_normalArray[posSize.y - sizeDivide2][posSize.x + sizeDivide2]
						+ m_normalArray[posSize.y + sizeDivide2][posSize.x + sizeDivide2])/2;
				}
			}
			else
			{
				m_vertexArray[m_vertexIndex + 5].position.y = m_elevArray[posSize.y][posSize.x + sizeDivide2];
				m_vertexArray[m_vertexIndex + 5].normal = m_normalArray[posSize.y][posSize.x + sizeDivide2];
			}
			m_vertexArray[m_vertexIndex + 5].tu = m_tuArray[posSize.x + sizeDivide2];
			m_vertexArray[m_vertexIndex + 5].tv = m_tvArray[posSize.y];

			// 左下角顶点
			m_vertexArray[m_vertexIndex + 6].position.x = left;
			m_vertexArray[m_vertexIndex + 6].position.z = bottom;
			m_vertexArray[m_vertexIndex + 6].position.y = m_elevArray[posSize.y + sizeDivide2][posSize.x - sizeDivide2];
			m_vertexArray[m_vertexIndex + 6].normal   = m_normalArray[posSize.y + sizeDivide2][posSize.x - sizeDivide2];
			m_vertexArray[m_vertexIndex + 6].tu = m_tuArray[posSize.x - sizeDivide2];
			m_vertexArray[m_vertexIndex + 6].tv = m_tvArray[posSize.y + sizeDivide2];

			// 下边顶点
			// 判断它左方的那个相邻节点的 m_bTouched 属性是否为true,如为真从数组中读取高程值，否则取相邻顶点高程值的平均值
			m_vertexArray[m_vertexIndex + 7].position.x = centerX;
			m_vertexArray[m_vertexIndex + 7].position.z = bottom;
			pAdjNode = FindNode( posSize.x, posSize.y + posSize.size );
			if( pAdjNode != NULL )
			{
				if( pAdjNode->m_bTouched )
				{
					m_vertexArray[m_vertexIndex + 7].position.y = m_elevArray[posSize.y + sizeDivide2][posSize.x];
					m_vertexArray[m_vertexIndex + 7].normal   = m_normalArray[posSize.y + sizeDivide2][posSize.x];
				}
				else
				{
					m_vertexArray[m_vertexIndex + 7].position.y = ( m_elevArray[posSize.y + sizeDivide2][posSize.x - sizeDivide2]
						+ m_elevArray[posSize.y + sizeDivide2][posSize.x + sizeDivide2])/2;
					m_vertexArray[m_vertexIndex + 7].normal = ( m_normalArray[posSize.y + sizeDivide2][posSize.x - sizeDivide2]
						+ m_normalArray[posSize.y + sizeDivide2][posSize.x + sizeDivide2])/2;
				}
			}
			else
			{
				m_vertexArray[m_vertexIndex + 7].position.y = m_elevArray[posSize.y + sizeDivide2][posSize.x];
				m_vertexArray[m_vertexIndex + 7].normal   = m_normalArray[posSize.y + sizeDivide2][posSize.x];
			}
			m_vertexArray[m_vertexIndex + 7].tu = m_tuArray[posSize.x];
			m_vertexArray[m_vertexIndex + 7].tv = m_tvArray[posSize.y + sizeDivide2];

			// 右下角顶点
			m_vertexArray[m_vertexIndex + 8].position.x = right;
			m_vertexArray[m_vertexIndex + 8].position.z = bottom;
			m_vertexArray[m_vertexIndex + 8].position.y = m_elevArray[posSize.y + sizeDivide2][posSize.x + sizeDivide2];
			m_vertexArray[m_vertexIndex + 8].normal   = m_normalArray[posSize.y + sizeDivide2][posSize.x + sizeDivide2];
			m_vertexArray[m_vertexIndex + 8].tu = m_tuArray[posSize.x + sizeDivide2];
			m_vertexArray[m_vertexIndex + 8].tv = m_tvArray[posSize.y + sizeDivide2];
		
			for(UINT i=0;i<9;i++)
			{
				m_vertexArray[m_vertexIndex+i].color = 0xffffffff;
			}
			// 索引数组赋值，   0 1 2   ，顺时针旋转
			//                  3 4 5 
			//                  6 7 8
			m_indexArray[m_indexIndex + 0]  = m_vertexIndex + 0;
			m_indexArray[m_indexIndex + 1]  = m_vertexIndex + 1;
			m_indexArray[m_indexIndex + 2]  = m_vertexIndex + 4;
			m_indexArray[m_indexIndex + 3]  = m_vertexIndex + 1;
			m_indexArray[m_indexIndex + 4]  = m_vertexIndex + 2;
			m_indexArray[m_indexIndex + 5]  = m_vertexIndex + 4;
			m_indexArray[m_indexIndex + 6]  = m_vertexIndex + 2;
			m_indexArray[m_indexIndex + 7]  = m_vertexIndex + 5;
			m_indexArray[m_indexIndex + 8]  = m_vertexIndex + 4;
			m_indexArray[m_indexIndex + 9]  = m_vertexIndex + 5;
			m_indexArray[m_indexIndex + 10] = m_vertexIndex + 8;
			m_indexArray[m_indexIndex + 11] = m_vertexIndex + 4;
			m_indexArray[m_indexIndex + 12] = m_vertexIndex + 8;
			m_indexArray[m_indexIndex + 13] = m_vertexIndex + 7;
			m_indexArray[m_indexIndex + 14] = m_vertexIndex + 4;
			m_indexArray[m_indexIndex + 15] = m_vertexIndex + 7;
			m_indexArray[m_indexIndex + 16] = m_vertexIndex + 6;
			m_indexArray[m_indexIndex + 17] = m_vertexIndex + 4;
			m_indexArray[m_indexIndex + 18] = m_vertexIndex + 6;
			m_indexArray[m_indexIndex + 19] = m_vertexIndex + 3;
			m_indexArray[m_indexIndex + 20] = m_vertexIndex + 4;
			m_indexArray[m_indexIndex + 21] = m_vertexIndex + 3;
			m_indexArray[m_indexIndex + 22] = m_vertexIndex + 0;
			m_indexArray[m_indexIndex + 23] = m_vertexIndex + 4;

			m_vertexIndex += 9;
			m_indexIndex  += 24;
		}
		else
		{
			RefineNode( pNode->m_pTLChildNode );
			RefineNode( pNode->m_pBLChildNode );
			RefineNode( pNode->m_pTRChildNode );
			RefineNode( pNode->m_pBRChildNode );
		}
	}
}

// 查找相对位置为(x, y)的那个节点
CLODTrnNode* CTerrain::FindNode(UINT x, UINT y)
{
	CLODTrnNode* pNode = m_pHeadNode;
	while(1)
	{
		if( pNode == NULL )
			return NULL;
		NODEPOSSIZE posSize = pNode->GetPosSize();
		if( posSize.x == x )
			return pNode; // 找到该节点，返回它的父节点
		else if( posSize.x < x )
		{
			if( posSize.y < y )
				pNode = pNode->m_pBRChildNode;
			else //posSize.y > y
				pNode = pNode->m_pTRChildNode;
		}
		else //posSize.x > x
		{
			if( posSize.y < y )
				pNode = pNode->m_pBLChildNode;
			else //posSize.y > y
				pNode = pNode->m_pTLChildNode;
		}
	}
}
void  CTerrain::RenderTerrain(BOOL bFog )
{
	// pre rendering
	D3DXVECTOR4 lightDirVec(3.0f, 3.0f, 0, 0);

	// set vertex shader constant table
	D3DXMATRIXA16 mWorld, mView, mProj;
	m_pd3dDevice->GetTransform(D3DTS_WORLD, &mWorld);
	m_pd3dDevice->GetTransform(D3DTS_VIEW, &mView);
	m_pd3dDevice->GetTransform(D3DTS_PROJECTION, &mProj);
	D3DXMATRIXA16 mWorldViewProj = mWorld * mView * mProj;
	D3DXMATRIXA16 mWorldView = mWorld * mView;
	//初始化HLSL变量
	m_pVConstTable->SetMatrix(m_pd3dDevice, "mWorldViewProj", &mWorldViewProj);
	m_pVConstTable->SetMatrix(m_pd3dDevice, "mWorldView", &mWorldView);
	m_pVConstTable->SetFloat( m_pd3dDevice, "lowest", 100.0f);
	m_pVConstTable->SetFloat( m_pd3dDevice, "highest", 140.0f);
	m_pVConstTable->SetFloat( m_pd3dDevice, "ambient", 0.5f);
	m_pVConstTable->SetVector(m_pd3dDevice, "lightDir", &lightDirVec);

	//// set pixel shader constant table
	//D3DXHANDLE hLowSmpl =  m_pPConstTable->GetConstantByName(NULL, "lowTexSmpl");
	//D3DXHANDLE hHighSmpl = m_pPConstTable->GetConstantByName(NULL, "highTexSmpl");
	//D3DXCONSTANT_DESC lowSmplDesc, highSmplDesc;
	//UINT count;
	//m_pPConstTable->GetConstantDesc(hLowSmpl, &lowSmplDesc, &count);
	//m_pPConstTable->GetConstantDesc(hHighSmpl,&highSmplDesc,&count);

	//// set low texture
	//m_pd3dDevice->SetTexture(lowSmplDesc.RegisterIndex, m_pTexture);
	//m_pd3dDevice->SetSamplerState(lowSmplDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//m_pd3dDevice->SetSamplerState(lowSmplDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//m_pd3dDevice->SetSamplerState(lowSmplDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//m_pd3dDevice->SetSamplerState(lowSmplDesc.RegisterIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
	//m_pd3dDevice->SetSamplerState(lowSmplDesc.RegisterIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
	//// set high texture
	//m_pd3dDevice->SetTexture(highSmplDesc.RegisterIndex, m_pTexture1);
	//m_pd3dDevice->SetSamplerState(highSmplDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//m_pd3dDevice->SetSamplerState(highSmplDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//m_pd3dDevice->SetSamplerState(highSmplDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//m_pd3dDevice->SetSamplerState(highSmplDesc.RegisterIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
	//m_pd3dDevice->SetSamplerState(highSmplDesc.RegisterIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);

	// rendering
	m_pd3dDevice->SetFVF( D3DFVF_TRNVERTEX );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(TRNVERTEX) );
	if(m_pTexture != NULL)
	{
		m_pd3dDevice->SetTexture( 0, m_pTexture );//设置纹理
		m_pd3dDevice->SetTexture( 1, m_pTexture1 );//设置纹理
	}
	m_pd3dDevice->SetVertexShader(m_pVShader);
	//m_pd3dDevice->SetPixelShader(m_pPShader);

	m_pd3dDevice->SetIndices( m_pIB );
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0/*base index index*/, 0,/*MinVertexIndex*/
		(m_numNodeRendered << 3 ) + m_numNodeRendered/*NumVertices*/,
		0/*StartIndex*/,
		(m_numNodeRendered << 3 )/*PrimitiveCount*/);

	// post rendering
	m_pd3dDevice->SetVertexShader(NULL);
	//m_pd3dDevice->SetPixelShader(NULL);
}

// 给定X坐标和Y坐标，求出该点的地形高度  未完
float CTerrain::GetTerrainElev(float x, float z)
{
	/*float elev;*/
	// 先求得对应于点 (x, z) 的相对坐标
	float xFloat, zFloat;
	xFloat = (x - m_centerPt.x) / m_GridWidth + ((m_width - 1)>>1);
	zFloat = (z - m_centerPt.y) / m_GridHeight + ((m_width - 1)>>1);
	int i, j;
	j = (int)xFloat;  // 取下整数
	i = (int)zFloat;  // 取下整数
	if(i<0 || i >= m_width || j < 0 || j >=m_width ) return 0;
	return m_elevArray[i][j];
}
