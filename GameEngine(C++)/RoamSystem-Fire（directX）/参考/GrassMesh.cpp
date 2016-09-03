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
		MessageBox(NULL,"��ȡ��λͼʧ��",NULL,MB_OK);
	}

	if(FAILED(Device->CreateVertexBuffer(12* sizeof(GRASSVERTEX), 0, GRASSVERTEX_FVF, D3DPOOL_MANAGED,&VertexBuffer, 0)))//3���棬12������
	{
		MessageBox(NULL,"�����ݶ������","",1);
	}
	GRASSVERTEX *grassvertex;
	VertexBuffer->Lock(0, 0, (void**)&grassvertex, 0);

	//��һ����
	grassvertex[0] = GRASSVERTEX(32.0f, 0.0f, 0.0f,  1.0f, 1.0f);
	grassvertex[1] = GRASSVERTEX(32.0f, 64.0f, 0.0f, 1.0f, 0.0f);
	grassvertex[2] = GRASSVERTEX(-32.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	grassvertex[3] = GRASSVERTEX(-32.0f, 64.0f, 0.0f,  0.0f, 0.0f);

	//�ڶ�����
	grassvertex[4] = GRASSVERTEX(32.0f, 0.0f, -22.0f,  1.0f, 1.0f);
	grassvertex[5] = GRASSVERTEX(32.0f, 64.0f, -22.0f,  1.0f, 0.0f);
	grassvertex[6] = GRASSVERTEX(-32.0f, 0.0f, 22.0f,  0.0f, 1.0f);
	grassvertex[7] = GRASSVERTEX(-32.0f, 64.0f, 22.0f,  0.0f, 0.0f);

	//��������
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
	Device->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);//����Դ�������Ϊ��As,As,As,As)   
	Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);//����Ŀ��������Ϊ��1-As,1-As,1-As,1-As);   
	Device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );//����ALPHA���Թ���   
	Device->SetRenderState( D3DRS_ALPHAREF,        0x0f );//����ALPHA���Բο�ֵ   
	Device->SetRenderState( D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL );//����APLHA���ԱȽϹ���
	for(int i = 0; i<3; i++)
	{
		
		Device->SetTexture(0, GrassMeshtex);
		Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,i*4,2);
	}
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
}
