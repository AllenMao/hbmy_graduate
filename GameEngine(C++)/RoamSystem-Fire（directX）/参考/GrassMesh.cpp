#include "GrassMesh.h"

GrassMesh::GrassMesh()
{
}

GrassMesh::~GrassMesh(void)
{
}

void GrassMesh::InitMesh(IDirect3DDevice9 *Device)
{
	if(FAILED(D3DXCreateTextureFromFile(Device,"res/texture/grass.png",&GrassMeshtex)))
	{
		MessageBox(NULL,"读取草位图失败",NULL,MB_OK);
	}

	if(FAILED(Device->CreateVertexBuffer(12* sizeof(GRASSVERTEX), 0, GRASSVERTEX_FVF, D3DPOOL_MANAGED,&VertexBuffer, 0)))//3个面，12个顶点
	{
		MessageBox(NULL,"创建草顶点出错","",1);
	}
	GRASSVERTEX *grassvertex;
	VertexBuffer->Lock(0, 0, (void**)&grassvertex, 0);

	//第一个面
	grassvertex[0] = GRASSVERTEX(32.0f, 0.0f, 0.0f,  1.0f, 1.0f);
	grassvertex[1] = GRASSVERTEX(32.0f, 64.0f, 0.0f, 1.0f, 0.0f);
	grassvertex[2] = GRASSVERTEX(-32.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	grassvertex[3] = GRASSVERTEX(-32.0f, 64.0f, 0.0f,  0.0f, 0.0f);

	//第二个面
	grassvertex[4] = GRASSVERTEX(32.0f, 0.0f, -22.0f,  1.0f, 1.0f);
	grassvertex[5] = GRASSVERTEX(32.0f, 64.0f, -22.0f,  1.0f, 0.0f);
	grassvertex[6] = GRASSVERTEX(-32.0f, 0.0f, 22.0f,  0.0f, 1.0f);
	grassvertex[7] = GRASSVERTEX(-32.0f, 64.0f, 22.0f,  0.0f, 0.0f);

	//第三个面
	grassvertex[8] = GRASSVERTEX(32.0f, 0.0f, 22.0f,  1.0f, 1.0f);
	grassvertex[9] = GRASSVERTEX(32.0f, 64.0f, 22.0f,  1.0f, 0.0f);
	grassvertex[10] = GRASSVERTEX(-32.0f, 0.0f, -22.0f, 0.0f, 1.0f);
	grassvertex[11] = GRASSVERTEX(-32.0f, 64.0f, -22.0f, 0.0f, 0.0f);

	VertexBuffer->Unlock();
}

void GrassMesh::DrawMesh(IDirect3DDevice9 *Device,float posx,float posy,float posz)
{
	D3DXMATRIX grassposmat;
	D3DXMatrixTranslation(&grassposmat,posx,posy,posz);
	D3DXMatrixScaling(&grassposmat,2.0f,2.0f,2.0f);
	Device->SetTransform(D3DTS_WORLD, &grassposmat);
	Device->SetStreamSource(0,this->VertexBuffer,0,sizeof(GRASSVERTEX));
	Device->SetFVF(GRASSVERTEX_FVF);
	Device->SetRenderState(D3DRS_LIGHTING, false);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	Device->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);//设置源混合因子为（As,As,As,As)   
	Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);//设置目标混合因子为（1-As,1-As,1-As,1-As);   
	Device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );//开启ALPHA测试功能   
	Device->SetRenderState( D3DRS_ALPHAREF,        0x0f );//设置ALPHA测试参考值   
	Device->SetRenderState( D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL );//设置APLHA测试比较规则
	for(int i = 0; i<3; i++)
	{
		
		Device->SetTexture(0, GrassMeshtex);
		Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,i*4,2);
	}
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
}
