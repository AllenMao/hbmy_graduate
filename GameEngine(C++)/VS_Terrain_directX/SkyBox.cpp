#include "SkyBox.h"

SkyBox::SkyBox(IDirect3DDevice9* Device)
{
	m_pd3dDevice = Device;
	m_fHeight = -800;
}

SkyBox::~SkyBox(void)
{
}
bool SkyBox::InitSky( LPCSTR fontFileName, LPCSTR backFileName, LPCSTR leftFileName, LPCSTR rightFileName, 
	LPCSTR topFileName, LPCSTR bottomFileName, float skywidth, float skyheight)
{
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,fontFileName,&Skytex[0])))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,backFileName,&Skytex[1])))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,leftFileName,&Skytex[2])))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,rightFileName,&Skytex[3])))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,topFileName,&Skytex[4])))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,bottomFileName,&Skytex[5])))
	{
		return false;
	}
	if(FAILED(m_pd3dDevice->CreateVertexBuffer(24* sizeof(SKYVERTEX), 0, SKYVERTEX_FVF, D3DPOOL_MANAGED,&m_pVertexBuffer, 0)))
	{
		MessageBox(NULL,"","",1);
	}
	SKYVERTEX *skyvertex;
	m_pVertexBuffer->Lock(0, 0, (void**)&skyvertex, 0);
	//ǰ��
	skyvertex[0] = SKYVERTEX(skywidth/2.0f, 0.0f+m_fHeight, -skywidth/2.0f, 1.0f, 1.0f);//��E
	skyvertex[1] = SKYVERTEX(skywidth/2.0f, skyheight+m_fHeight, -skywidth/2.0f, 1.0f, 0.0f);//��F
	skyvertex[2] = SKYVERTEX(-skywidth/2.0f, 0.0f+m_fHeight, -skywidth/2.0f, 0.0f, 1.0f);//��O
	skyvertex[3] = SKYVERTEX(-skywidth/2.0f, skyheight+m_fHeight, -skywidth/2.0f, 0.0f, 0.0f);//��G
	//����
	skyvertex[4] = SKYVERTEX(-skywidth/2.0f, 0.0f+m_fHeight, skywidth/2.0f, 1.0f, 1.0f);//��D
	skyvertex[5] = SKYVERTEX(-skywidth/2.0f, skyheight+m_fHeight, skywidth/2.0f, 1.0f, 0.0f);//��A
	skyvertex[6] = SKYVERTEX(skywidth/2.0f, 0.0f+m_fHeight, skywidth/2.0f, 0.0f, 1.0f);//��C
	skyvertex[7] = SKYVERTEX(skywidth/2.0f, skyheight+m_fHeight, skywidth/2.0f, 0.0f, 0.0f);//��B
	//����
	skyvertex[8] = SKYVERTEX(-skywidth/2.0f, 0.0f+m_fHeight, -skywidth/2.0f, 1.0f, 1.0f);
	skyvertex[9] = SKYVERTEX(-skywidth/2.0f, skyheight+m_fHeight, -skywidth/2.0f, 1.0f, 0.0f);//��G
	skyvertex[10] = SKYVERTEX(-skywidth/2.0f, 0.0f+m_fHeight, skywidth/2.0f, 0.0f, 1.0f);//��D
	skyvertex[11] = SKYVERTEX(-skywidth/2.0f, skyheight+m_fHeight, skywidth/2.0f, 0.0f, 0.0f);//��A
	//����
	skyvertex[12] = SKYVERTEX(skywidth/2.0f, 0.0f+m_fHeight, skywidth/2.0f, 1.0f, 1.0f);//��C
	skyvertex[13] = SKYVERTEX(skywidth/2.0f, skyheight+m_fHeight, skywidth/2.0f, 1.0f, 0.0f);//��B
	skyvertex[14] = SKYVERTEX(skywidth/2.0f, 0.0f+m_fHeight, -skywidth/2.0f, 0.0f, 1.0f);//��E
	skyvertex[15] = SKYVERTEX(skywidth/2.0f, skyheight+m_fHeight, -skywidth/2.0f, 0.0f, 0.0f);//��F
	//����
	skyvertex[16] = SKYVERTEX(skywidth/2.0f, skyheight+m_fHeight, -skywidth/2.0f, 1.0f, 1.0f);//��F
	skyvertex[17] = SKYVERTEX(skywidth/2.0f, skyheight+m_fHeight, skywidth/2.0f, 1.0f, 0.0f);//��B
	skyvertex[18] = SKYVERTEX(-skywidth/2.0f, skyheight+m_fHeight, -skywidth/2.0f, 0.0f, 1.0f);//��G
	skyvertex[19] = SKYVERTEX(-skywidth/2.0f, skyheight+m_fHeight, skywidth/2.0f, 0.0f, 0.0f);//��A
	//����
	skyvertex[20] = SKYVERTEX(-skywidth/2.0f, 0.0f+m_fHeight, -skywidth/2.0f, 1.0f, 1.0f);//��O
	skyvertex[21] = SKYVERTEX(-skywidth/2.0f, 0.0f+m_fHeight, skywidth/2.0f, 1.0f, 0.0f);//��D
	skyvertex[22] = SKYVERTEX(skywidth/2.0f, 0.0f+m_fHeight, -skywidth/2.0f, 0.0f, 1.0f);//��E
	skyvertex[23] = SKYVERTEX(skywidth/2.0f, 0.0f+m_fHeight, skywidth/2.0f, 0.0f, 0.0f);//��C
	m_pVertexBuffer->Unlock();
	return true;
}

void SkyBox::Render( D3DXMATRIX *pos)
{
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//��ʼ��պ���
	m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50,50,50));
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	m_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRRORONCE);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRRORONCE);

	m_pd3dDevice->SetTransform(D3DTS_WORLD, pos);
	m_pd3dDevice->SetStreamSource(0,this->m_pVertexBuffer,0,sizeof(SKYVERTEX));
	m_pd3dDevice->SetFVF(SKYVERTEX_FVF);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	for(int i = 0; i<6; i++)
	{
		m_pd3dDevice->SetTexture(0, Skytex[i]);
		m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,i*4,2);
	}
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);
	/*m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, 0);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, 0);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, 0);*/
}