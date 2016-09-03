#include "WaterPlane.h"

CWaterPlane::CWaterPlane(IDirect3DDevice9* Device)
{
	m_pd3dDevice = Device;
	m_pVBPool = NULL;
	m_pIBPool = NULL;
	m_texInsidePool = NULL;
	m_iNumVerts = 0;

	m_fHeightArray1 = NULL;
	m_fHeightArray2 = NULL;

	m_fDampValue = 1.1f;
	m_fRefractionIndex = 1.33f;
	m_fDepth = 1.0f;
	m_bHardcodeNormals = true;
	m_fEnvBlendFactor = 0.6f;
}

CWaterPlane::~CWaterPlane()
{

}

HRESULT CWaterPlane::CreateVertexGrid(D3DPOOL pool, float fTotalSize, DWORD dwColor, 
	int iNumVerticesX, int iNumVerticesY)
{
	HRESULT hr;

	// create and fill vertex buffer
	{

		m_pd3dDevice->CreateVertexBuffer(iNumVerticesX*iNumVerticesY*sizeof(VERTEX_XYZ_NORMAL_DIFFUSE_TEX2),
			0, D3DFVF_XYZ_NORMAL_DIFFUSE_TEX2, D3DPOOL_MANAGED, &m_pVBPool, NULL);

		VERTEX_XYZ_NORMAL_DIFFUSE_TEX2 *pVertices;
		float fSizeDiv2 = fTotalSize/2;

		m_pVBPool->Lock( 0, 0, (void**)&pVertices, 0 );
		for (int x=0; x < iNumVerticesX; x++) 
		{
			for (int y=0; y < iNumVerticesY; y++) 
			{
				pVertices[(y*iNumVerticesX)+x].position = D3DXVECTOR3(
					(iNumVerticesX > 1) ? (((float)x/(float)(iNumVerticesX-1))*fTotalSize)-fSizeDiv2 : 0, 
					0.0f,
					(iNumVerticesY > 1) ? (((float)(iNumVerticesY-1-y)/(float)(iNumVerticesY-1))*fTotalSize)-fSizeDiv2 : 0);

				pVertices[(y*iNumVerticesX)+x].color    = dwColor;
				pVertices[(y*iNumVerticesX)+x].tu2       = (float)x/(float)(iNumVerticesX-1);
				pVertices[(y*iNumVerticesX)+x].tv2       = (float)y/(float)(iNumVerticesY-1);

			}
		}
	}

	// create index buffer
	{
		m_pd3dDevice->CreateIndexBuffer(iNumVerticesX*iNumVerticesY*2*3*2, // *2 (tris) *3 (indicies per tri) * 2 (bytes per index)
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,D3DPOOL_MANAGED, &m_pIBPool, 0);
		// lock and fill index buffer
		WORD *pIndices = NULL;
		m_pIBPool->Lock(0, 0, (void**)&pIndices, 0);
		int iIndex = 0;
		for (int y=0; y < iNumVerticesY-1; y++) 
		{
			for (int x=0; x < iNumVerticesX-1; x++) 
			{
				// first triangle
				pIndices[iIndex] = ((y)*iNumVerticesX)+x;
				pIndices[iIndex+1] = ((y)*iNumVerticesX)+x+1;
				pIndices[iIndex+2] = ((y+1)*iNumVerticesX)+x;
				/**(pIndex++) = ((y)*iNumVerticesX)+x;
				*(pIndex++) = ((y)*iNumVerticesX)+x+1;
				*(pIndex++) = ((y+1)*iNumVerticesX)+x+1;*/

				// second triangle
				/**(pIndex++) = ((y)*iNumVerticesX)+x;
				*(pIndex++) = ((y+1)*iNumVerticesX)+x+1;
				*(pIndex++) = ((y+1)*iNumVerticesX)+x;*/

				pIndices[iIndex+3] = ((y+1)*iNumVerticesX)+x;
				pIndices[iIndex+4] = ((y)*iNumVerticesX)+x+1;
				pIndices[iIndex+5] = ((y+1)*iNumVerticesX)+x+1;
				iIndex += 6;  //Ë÷Òý×Ô¼Ó6
			}
		}
		m_pIBPool->Unlock();
		m_pVBPool->Unlock();
	}
	return(S_OK);
}

HRESULT CWaterPlane::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pDev,
	int iNumVerts, const char *strInPoolTextureFilename)
{
	m_iNumVerts = iNumVerts;
	m_pd3dDevice = pDev;

	if (FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, strInPoolTextureFilename, &m_texInsidePool)))
		return -1;

	CreateVertexGrid(D3DPOOL_MANAGED, 1.0f, 
		D3DXCOLOR(0.0f, 2.0f, 0.0f, 1.0f), iNumVerts, iNumVerts);

	// initialize height arrays
	m_fHeightArray1 = new float[iNumVerts*iNumVerts];
	m_fHeightArray2 = new float[iNumVerts*iNumVerts];

	memset(m_fHeightArray1, 0, sizeof(float)*iNumVerts*iNumVerts);
	memset(m_fHeightArray2, 0, sizeof(float)*iNumVerts*iNumVerts);

	m_pActiveHeightArray = m_fHeightArray1;
	m_pScratchHeightArray = m_fHeightArray2;

	m_Timer.InitTimer();

	return(S_OK);
}

void CWaterPlane::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_texInsidePool);
	SAFE_RELEASE(m_pVBPool);
	SAFE_RELEASE(m_pIBPool);

	SAFE_DELETE_ARRAY(m_fHeightArray1);
	SAFE_DELETE_ARRAY(m_fHeightArray2);
}

/****************************************************************************

ProcessWater: this function processes our water.  It takes two input buffers,
the water dimensions, and the cooling amount.  It calculates the new water
values from waterfield1 and puts them into waterfield2.

****************************************************************************/
void CWaterPlane::ProcessWater()
{
	// loop through all the water values...
	for (int y=0; y < m_iNumVerts; y++) 
	{
		for (int x=0; x < m_iNumVerts; x++) 
		{

			// add up the values of all the neighboring water values...
			float value;
			int xminus1 = x-1; if (xminus1 < 0) xminus1 = 0;
			int xminus2 = x-2; if (xminus2 < 0) xminus2 = 0;
			int yminus1 = y-1; if (yminus1 < 0) yminus1 = 0;
			int yminus2 = y-2; if (yminus2 < 0) yminus2 = 0;

			int xplus1 = x+1; if (xplus1 >= m_iNumVerts) xplus1 = (m_iNumVerts)-1;
			int xplus2 = x+2; if (xplus2 >= m_iNumVerts) xplus2 = (m_iNumVerts)-1;
			int yplus1 = y+1; if (yplus1 >= m_iNumVerts) yplus1 = (m_iNumVerts)-1;
			int yplus2 = y+2; if (yplus2 >= m_iNumVerts) yplus2 = (m_iNumVerts)-1;

			// Method 1: Slower but yields slightly better looking water
			{

				value  = m_pActiveHeightArray[((y)      *m_iNumVerts)+xminus1];
				value += m_pActiveHeightArray[((y)      *m_iNumVerts)+xminus2];
				value += m_pActiveHeightArray[((y)      *m_iNumVerts)+xplus1];
				value += m_pActiveHeightArray[((y)      *m_iNumVerts)+xplus2];
				value += m_pActiveHeightArray[((yminus1)*m_iNumVerts)+x];
				value += m_pActiveHeightArray[((yminus2)*m_iNumVerts)+x];
				value += m_pActiveHeightArray[((yplus1) *m_iNumVerts)+x];
				value += m_pActiveHeightArray[((yplus2) *m_iNumVerts)+x];
				value += m_pActiveHeightArray[((yminus1)*m_iNumVerts)+xminus1];
				value += m_pActiveHeightArray[((yminus1)*m_iNumVerts)+xplus1];
				value += m_pActiveHeightArray[((yplus1) *m_iNumVerts)+xminus1];
				value += m_pActiveHeightArray[((yplus1) *m_iNumVerts)+xplus1];

				// average them
				value /= 6.0f;

			}

			// subtract the previous water value
			value -= m_pScratchHeightArray[(y*m_iNumVerts)+x];

			// dampen it!
			value /= m_fDampValue;

			if (value > 10.0f) value = 10.0f;
			if (value < -10.0f) value = -10.0f;

			// store it in array
			m_pScratchHeightArray[(y*m_iNumVerts)+x] = value;
		}
	}
}

float CWaterPlane::CalcDisplacement(float fHeightdiff)
{
	// the angle is the arctan of the height difference
	float angle = (float)atan(fHeightdiff);

	// now, calculate the angle of the refracted beam.
	float beamangle = (float)asin(sin(angle) / m_fRefractionIndex);

	// finally, calculate the displacement, based on the refracted beam
	// and the height difference.
	return(tan(beamangle) * (fHeightdiff+m_fDepth));
}

void CWaterPlane::ApplyHeightArrayToVB()
{
	HRESULT hr;
	float fSizeDiv2 = 0.5f;

	VERTEX_XYZ_NORMAL_DIFFUSE_TEX2 *pVertices = NULL;
	m_pVBPool->Lock( 0, 0, (void**)&pVertices, 0 );

	int x,y;
	for (x=0; x < m_iNumVerts; x++) 
	{
		for (y=0; y < m_iNumVerts; y++) 
		{
			float fValue = m_pActiveHeightArray[(y*m_iNumVerts)+x];
			if (fValue > 2.0f) fValue = 2.0f;


			pVertices[(y*m_iNumVerts)+x].position = D3DXVECTOR3(
				(m_iNumVerts > 1) ? (((float)x/(float)(m_iNumVerts-1)))-fSizeDiv2 : 0, 
				fValue+180.0f,
				(m_iNumVerts > 1) ? (((float)(m_iNumVerts-1-y)/(float)(m_iNumVerts-1)))-fSizeDiv2 : 0);

			// calculate texture coordinates
			{
				float xdiff = (x == m_iNumVerts-1) ? 0 : 
					m_pActiveHeightArray[(y*m_iNumVerts)+x+1] - 
					m_pActiveHeightArray[(y*m_iNumVerts)+x];

				float ydiff = (y == m_iNumVerts-1) ? 0 : 
					m_pActiveHeightArray[((y+1)*m_iNumVerts)+x] - 
					m_pActiveHeightArray[(y*m_iNumVerts)+x];

				float xdisp = CalcDisplacement(xdiff);
				float ydisp = CalcDisplacement(ydiff);

				float tu, tv;

				if (xdiff < 0) 
				{
					if (ydiff < 0) 
					{
						tu = (float)x/(float)(m_iNumVerts-1)-xdisp;
						tv = (float)y/(float)(m_iNumVerts-1)-ydisp;
					}
					else 
					{
						tu = (float)x/(float)(m_iNumVerts-1)-xdisp;
						tv = (float)y/(float)(m_iNumVerts-1)+ydisp;
					}
				}
				else 
				{
					if (ydiff < 0) 
					{
						tu = (float)x/(float)(m_iNumVerts-1)+xdisp;
						tv = (float)y/(float)(m_iNumVerts-1)-ydisp;
					}
					else 
					{
						tu = (float)x/(float)(m_iNumVerts-1)+xdisp;
						tv = (float)y/(float)(m_iNumVerts-1)+ydisp;
					}
				}

				if (tu < 0.0f) tu = 0.0f; if (tu > 1.0f) tu = 1.0f;
				if (tv < 0.0f) tv = 0.0f; if (tv > 1.0f) tv = 1.0f;

				pVertices[(y*m_iNumVerts)+x].tu2 = tu;
				pVertices[(y*m_iNumVerts)+x].tv2 = tv;
				pVertices[(y*m_iNumVerts)+x].color = 
					D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fEnvBlendFactor);
			}
		}
	}


	// since our model has changed, we must now recompute the normals.
	for (x=0; x < m_iNumVerts; x++) 
	{
		for (y=0; y < m_iNumVerts; y++) 
		{
			if (m_bHardcodeNormals) 
			{
				pVertices[(y*m_iNumVerts)+x].normal = D3DXVECTOR3(0,1,0);
			}
			else 
			{
				// calc normal
				int xplus1 = (x == m_iNumVerts-1) ? m_iNumVerts-2 : x+1;
				int yplus1 = (y == m_iNumVerts-1) ? m_iNumVerts-2 : y+1;

				D3DXVECTOR3 v1 = pVertices[(y*m_iNumVerts)+x].position;
				D3DXVECTOR3 v2 = pVertices[(y*m_iNumVerts)+xplus1].position;
				D3DXVECTOR3 v3 = pVertices[(yplus1*m_iNumVerts)+x].position;

				D3DXVECTOR3 vNorm1 = v2 - v1;
				D3DXVECTOR3 vNorm2 = v3 - v1;
				D3DXVECTOR3 vNorm;

				D3DXVec3Normalize(&vNorm1, &vNorm1);
				D3DXVec3Normalize(&vNorm2, &vNorm2);
				D3DXVec3Cross(&vNorm, &vNorm2, &vNorm1);

				D3DXVec3Normalize(&vNorm, &vNorm);

				pVertices[(y*m_iNumVerts)+x].normal = vNorm;
			}
		}
	}

	m_pVBPool->Unlock();
}

void CWaterPlane::CreateWaterDroplet(int iX, int iY, int iSize, 
	int iSplashStrength)
{
	for (int x=iX-iSize; x <= iX+iSize; x++) 
	{
		for (int y=iY-iSize; y <= iY+iSize; y++) 
		{
			// make sure we're in bounds
			if (x < 0 || x >= m_iNumVerts || y < 0 || y >= m_iNumVerts) continue;

			// see if the point at (x,y) is within the circle of radius size
			int square_x    = (x-iX)*(x-iX);
			int square_y    = (y-iY)*(y-iY);
			int square_size = iSize*iSize;

			if (square_x+square_y <= square_size) 
			{
				// it's within the size circle!  apply it to the water buffer.
				m_pActiveHeightArray[(y*m_iNumVerts)+(x)] += 
					(float)iSplashStrength * sqrt((double)square_x+square_y);
			}
		}
	}
}

void CWaterPlane::Update(float fElapsedTime)
{
	if (m_Timer.GetPlayTime() > 0.04) 
	{
		
		ProcessWater();
		ApplyHeightArrayToVB();


		// flip-flop the water buffers.
		float *temp = m_pActiveHeightArray;
		m_pActiveHeightArray = m_pScratchHeightArray;
		m_pScratchHeightArray = temp;
		m_Timer.InitTimer();
	}
}

void CWaterPlane::Render()
{
	 // draw water
	D3DLIGHT9 light; 
	ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Position = D3DXVECTOR3(-0.0f, 20.0f, -10.0f);
	light.Ambient.a = 1.0f; 
	light.Ambient.g = 0.4f;
	light.Ambient.b = 0.4f;
	light.Ambient.r = 0.4f;
	light.Diffuse.a = 1.0f; 
	light.Diffuse.g = 0.8f;
	light.Diffuse.b = 0.8f;
	light.Diffuse.r = 0.8f;
	light.Direction = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	m_pd3dDevice->SetLight(0, &light);
	m_pd3dDevice->LightEnable(0, true);
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);


	D3DXMATRIX matTrans, matScale,matWorld;
	D3DXMatrixTranslation(&matTrans, 0.0f, 1.1f, 0.0f);
	D3DXMatrixScaling(&matScale, 16.0f, 1.0f, 20.0f);
	matWorld = matScale * matTrans;
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

	// Turn on texture-coord generation for cubemapping
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
	//m_pd3dDevice->SetSimpleStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_MIRROR );D3DSAMP_ADDRESSU 
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_MIRROR );
	m_pd3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
	m_pd3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);

	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_BLENDDIFFUSEALPHA );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );


	m_pd3dDevice->SetTexture( 1, m_texInsidePool );
	m_pd3dDevice->SetStreamSource( 0, m_pVBPool, 0, sizeof(VERTEX_XYZ_NORMAL_DIFFUSE_TEX2) );
	m_pd3dDevice->SetFVF(D3DFVF_XYZ_NORMAL_DIFFUSE_TEX2);
	m_pd3dDevice->SetIndices( m_pIBPool);
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 
		0, m_iNumVerts*m_iNumVerts,0,m_iNumVerts*m_iNumVerts*2);

	//restore
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
}

///////////////////////////////////////////////////////////////////////
struct ANIMATINGTEX
{
	float _x,_y,_z;
	float _u,_v;
	ANIMATINGTEX(FLOAT x, FLOAT y, FLOAT z,FLOAT u, FLOAT v)  
                   : _x(x), _y(y), _z(z), _u(u),_v(v) {}
};
#define D3DFVF_ANIMATINGTEX (D3DFVF_XYZ |D3DFVF_TEX1)
void CWaterPlane::AnimatingTexture(char* szFileName)
{
	LPDIRECT3DVERTEXBUFFER9 pVB;
	LPDIRECT3DTEXTURE9 pTex;
	D3DXMATRIX matTexture;
	D3DXMatrixIdentity(&matTexture);
	if(szFileName != NULL)
		D3DXCreateTextureFromFile(m_pd3dDevice, szFileName, &pTex);

	m_pd3dDevice->CreateVertexBuffer( 4 * sizeof(ANIMATINGTEX), 0, 
		D3DFVF_ANIMATINGTEX, D3DPOOL_MANAGED, &pVB, NULL );
	ANIMATINGTEX *pVertices = NULL;

	pVB->Lock(0, 0, (void**)&pVertices, 0); 
	pVertices[0]= ANIMATINGTEX(-100.0f, 150.0f, 50.0f-60.0f, 0.0f, 1.0f);  
	pVertices[1]= ANIMATINGTEX(-100.0f, 150.0f, 150.0f-60.0f, 0.0f, 0.0f);  
	pVertices[2]= ANIMATINGTEX( 100.0f, 150.0f, 50.0f-60.0f, 1.0f, 1.0f);  
	pVertices[3]= ANIMATINGTEX( 100.0f, 150.0f, 150.0f-60.0f, 1.0f, 0.0f);
	pVB->Unlock();

	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,
        D3DTTFF_COUNT2);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	// Texture state
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

	matTexture._31 += m_Timer.GetPlayTime() /10000;
	m_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &matTexture) ;

	m_pd3dDevice->SetTexture( 0, pTex );
	m_pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(ANIMATINGTEX) );
	m_pd3dDevice->SetFVF( D3DFVF_ANIMATINGTEX );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0, 2 );

	m_pd3dDevice->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,
        D3DTTFF_DISABLE);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true);
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
}