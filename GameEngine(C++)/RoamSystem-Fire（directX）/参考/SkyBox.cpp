#include ".\SkyBox.h"

SkyBox::SkyBox()
{
}

SkyBox::~SkyBox(void)
{
}
bool SkyBox::InitSky(IDirect3DDevice9 *Device, LPCSTR fontFileName, LPCSTR backFileName, LPCSTR leftFileName, LPCSTR rightFileName, LPCSTR topFileName, LPCSTR bottomFileName, float skywidth, float skyheight)
{
	if(FAILED(D3DXCreateTextureFromFile(Device,fontFileName,&Skytex[0])))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(Device,backFileName,&Skytex[1])))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(Device,leftFileName,&Skytex[2])))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(Device,rightFileName,&Skytex[3])))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(Device,topFileName,&Skytex[4])))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(Device,bottomFileName,&Skytex[5])))
	{
		return false;
	}
	if(FAILED(Device->CreateVertexBuffer(24* sizeof(SKYVERTEX), 0, SKYVERTEX_FVF, D3DPOOL_MANAGED,&m_pVertexBuffer, 0)))
	{
		MessageBox(NULL,"","",1);
	}
	SKYVERTEX *skyvertex;
	m_pVertexBuffer->Lock(0, 0, (void**)&skyvertex, 0);
	//前面
	skyvertex[0] = SKYVERTEX(skywidth/2.0f, 0.0f, -skywidth/2.0f, 1.0f, 1.0f);//点E
	skyvertex[1] = SKYVERTEX(skywidth/2.0f, skyheight, -skywidth/2.0f, 1.0f, 0.0f);//点F
	skyvertex[2] = SKYVERTEX(-skywidth/2.0f, 0.0f, -skywidth/2.0f, 0.0f, 1.0f);//点O
	skyvertex[3] = SKYVERTEX(-skywidth/2.0f, skyheight, -skywidth/2.0f, 0.0f, 0.0f);//点G
	//后面
	skyvertex[4] = SKYVERTEX(-skywidth/2.0f, 0.0f, skywidth/2.0f, 1.0f, 1.0f);//点D
	skyvertex[5] = SKYVERTEX(-skywidth/2.0f, skyheight, skywidth/2.0f, 1.0f, 0.0f);//点A
	skyvertex[6] = SKYVERTEX(skywidth/2.0f, 0.0f, skywidth/2.0f, 0.0f, 1.0f);//点C
	skyvertex[7] = SKYVERTEX(skywidth/2.0f, skyheight, skywidth/2.0f, 0.0f, 0.0f);//点B
	//左面
	skyvertex[8] = SKYVERTEX(-skywidth/2.0f, 0.0f, -skywidth/2.0f, 1.0f, 1.0f);
	skyvertex[9] = SKYVERTEX(-skywidth/2.0f, skyheight, -skywidth/2.0f, 1.0f, 0.0f);//点G
	skyvertex[10] = SKYVERTEX(-skywidth/2.0f, 0.0f, skywidth/2.0f, 0.0f, 1.0f);//点D
	skyvertex[11] = SKYVERTEX(-skywidth/2.0f, skyheight, skywidth/2.0f, 0.0f, 0.0f);//点A
	//右面
	skyvertex[12] = SKYVERTEX(skywidth/2.0f, 0.0f, skywidth/2.0f, 1.0f, 1.0f);//点C
	skyvertex[13] = SKYVERTEX(skywidth/2.0f, skyheight, skywidth/2.0f, 1.0f, 0.0f);//点B
	skyvertex[14] = SKYVERTEX(skywidth/2.0f, 0.0f, -skywidth/2.0f, 0.0f, 1.0f);//点E
	skyvertex[15] = SKYVERTEX(skywidth/2.0f, skyheight, -skywidth/2.0f, 0.0f, 0.0f);//点F
	//顶面
	skyvertex[16] = SKYVERTEX(skywidth/2.0f, skyheight, -skywidth/2.0f, 1.0f, 1.0f);//点F
	skyvertex[17] = SKYVERTEX(skywidth/2.0f, skyheight, skywidth/2.0f, 1.0f, 0.0f);//点B
	skyvertex[18] = SKYVERTEX(-skywidth/2.0f, skyheight, -skywidth/2.0f, 0.0f, 1.0f);//点G
	skyvertex[19] = SKYVERTEX(-skywidth/2.0f, skyheight, skywidth/2.0f, 0.0f, 0.0f);//点A
	//底面
	skyvertex[20] = SKYVERTEX(-skywidth/2.0f, 0.0f, -skywidth/2.0f, 1.0f, 1.0f);//点O
	skyvertex[21] = SKYVERTEX(-skywidth/2.0f, 0.0f, skywidth/2.0f, 1.0f, 0.0f);//点D
	skyvertex[22] = SKYVERTEX(skywidth/2.0f, 0.0f, -skywidth/2.0f, 0.0f, 1.0f);//点E
	skyvertex[23] = SKYVERTEX(skywidth/2.0f, 0.0f, skywidth/2.0f, 0.0f, 0.0f);//点C
	m_pVertexBuffer->Unlock();
	return true;
}

void SkyBox::Render(IDirect3DDevice9 *Device, D3DXMATRIX *pos)
{
	Device->SetTransform(D3DTS_WORLD, pos);
	Device->SetStreamSource(0,this->m_pVertexBuffer,0,sizeof(SKYVERTEX));
	Device->SetFVF(SKYVERTEX_FVF);
	Device->SetRenderState(D3DRS_LIGHTING, false);
	for(int i = 0; i<6; i++)
	{
		Device->SetTexture(0, Skytex[i]);
		Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,i*4,2);
	}
	Device->SetRenderState(D3DRS_LIGHTING, true);
}