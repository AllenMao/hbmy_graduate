#include "MyGun.h"


MyGun::MyGun()
{
	firestate = 0;
}

MyGun::~MyGun(void)
{
}


void MyGun::Init(IDirect3DDevice9 *Device)
{
	if(FAILED(D3DXLoadMeshFromX("res\\model\\gun.x", D3DXMESH_MANAGED, Device, 
				&pAdjacencyBuffer, &pMaterialsBuffer, NULL, &numMaterials, &xmesh)))
	{
		MessageBox(NULL, "载入x模型文件失败", NULL, 1);
	}
	pD3DMaterial = (D3DXMATERIAL*)pMaterialsBuffer->GetBufferPointer();
	if(pD3DMaterial != NULL)
	{
		pMaterialsArray = new D3DMATERIAL9[numMaterials];
		pTextureArray = new IDirect3DTexture9*[numMaterials];
		for(int i=0; i<(int)numMaterials; i++)
		{
			pMaterialsArray[i] = pD3DMaterial[i].MatD3D;
			pMaterialsArray[i].Power = 1.0f;
			if(pD3DMaterial[i].pTextureFilename != NULL)
			{
				if(FAILED(D3DXCreateTextureFromFile(Device, pD3DMaterial[i].pTextureFilename, &pTextureArray[i])))
				{
					pTextureArray[i] = NULL;
				}
			}
			else
			{
				pTextureArray[i] = NULL;
			}
		}
	}
	pAdjacencyBuffer->Release();
	pMaterialsBuffer->Release();

	//创建MUZZLE方块
	Device->CreateVertexBuffer(6*sizeof(MUZZLEVERTEX),D3DUSAGE_WRITEONLY, MUZZLEVERTEX_FVF, D3DPOOL_MANAGED, &muzzle_VB, NULL);

	MUZZLEVERTEX *muzzleVertices;

	muzzle_VB->Lock( 0, 6*sizeof(MUZZLEVERTEX), (void**)&muzzleVertices, 0);

	//第一个三角形
	muzzleVertices[0].position = D3DXVECTOR3(-16, 16, 0.0f);
	muzzleVertices[0].tu       = 0.0f;
	muzzleVertices[0].tv       = 0.0f;

	muzzleVertices[1].position = D3DXVECTOR3(16, 16, 0.0f);
	muzzleVertices[1].tu       = 1.0f;
	muzzleVertices[1].tv       = 0.0f;

	muzzleVertices[2].position = D3DXVECTOR3(16, -16, 0.0f);
	muzzleVertices[2].tu       = 1.0f;
	muzzleVertices[2].tv       = 1.0f;

	//第二个三角
	muzzleVertices[3].position = D3DXVECTOR3(-16, 16, 0.0f);
	muzzleVertices[3].tu       = 0.0f;
	muzzleVertices[3].tv       = 0.0f;

	muzzleVertices[4].position = D3DXVECTOR3(16, -16, 0.0f);
	muzzleVertices[4].tu       = 1.0f;
	muzzleVertices[4].tv       = 1.0f;

	muzzleVertices[5].position = D3DXVECTOR3(-16, -16, 0.0f);
	muzzleVertices[5].tu       = 0.0f;
	muzzleVertices[5].tv       = 1.0f;

	muzzle_VB ->Unlock();

	D3DXCreateTextureFromFile(Device,"res\\texture\\muzzle.dds",&muzzle_tex);

}

void MyGun::Update(IDirect3DDevice9 *Device,CCamera *Camera )
{
	D3DXMATRIX matTrans,matScale,matRot,matMuzzleTrans;
	D3DXMATRIX matViewInverse;
	float fDet;
	//摄像机视图矩阵
	D3DXMATRIX matView = Camera->GetMatrix();
	//反转矩阵
	D3DXMatrixInverse(&matViewInverse,&fDet,&matView);
	//位移矩阵
	if(firestate == true)
	{
		D3DXMatrixTranslation(&matTrans,(rand()%1+8)*0.1f ,-1.6f,(rand()%4+4)*0.1f );
	}
	else
	{
		D3DXMatrixTranslation(&matTrans,0.85f ,-1.6f ,0.8f );
	}
	D3DXMatrixScaling(&matScale,3.0f,3.0f,3.0f);
	D3DXMatrixRotationYawPitchRoll(&matRot,-0.18f,-0.15f,0.2f);

	D3DXMatrixTranslation(&matMuzzleTrans, 0.0f, 0.15f, 3.5f);//对于枪，火焰的移动矩阵
	matResult = matRot * matScale * matTrans * matViewInverse;//枪最终矩阵

	matEmitterWorld = matTrans * matMuzzleTrans  *matRot* matViewInverse;//枪口矩阵  只要matViewInverse
	muzzlepos = D3DXVECTOR3(matEmitterWorld._41,matEmitterWorld._42,matEmitterWorld._43);//计算出枪口位置

	this->light = new CLight;
}

void MyGun::Render(IDirect3DDevice9 *Device,CCamera *Camera)
{

	Device->SetTransform(D3DTS_WORLD, &matResult);
	for(int i=0; i<(int)numMaterials; i++)
	{
		Device->SetTexture(0, pTextureArray[i]);
		Device->SetMaterial(&pMaterialsArray[i]);
		xmesh->DrawSubset(i);
	}

	if(firestate == true)
	{
		//画MUZZLE
		MuzzleScale = (rand()%7+3) *0.02f;//随机大小
		MuzzleRot = (rand()%7+3) *0.1f;//随机旋转
		D3DXMATRIX view,matScale, matTranspose, matBillboardRot, matWorld;
		D3DXMATRIX matRot; // user-specified rotation
		view = Camera->GetMatrix();
		D3DXMatrixTranspose(&matTranspose, &view);
		D3DXMatrixIdentity(&matBillboardRot);
  
		matBillboardRot._11 = matTranspose._11;
		matBillboardRot._12 = matTranspose._12;
		matBillboardRot._13 = matTranspose._13;
		matBillboardRot._21 = matTranspose._21;
		matBillboardRot._22 = matTranspose._22;
		matBillboardRot._23 = matTranspose._23;
		matBillboardRot._31 = matTranspose._31;
		matBillboardRot._32 = matTranspose._32;
		matBillboardRot._33 = matTranspose._33;
      
		D3DXMatrixTranslation( &matWorld, muzzlepos.x, muzzlepos.y, muzzlepos.z );
		D3DXMatrixRotationYawPitchRoll( &matRot, 0, 0, MuzzleRot );

		D3DXMatrixScaling(&matScale, MuzzleScale, MuzzleScale, 1);


		Device->SetRenderState( D3DRS_ALPHABLENDENABLE,true );
		Device->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);//设置源混合因子为（As,As,As,As)   
		Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);//设置目标混合因子为（1-As,1-As,1-As,1-As);   
		Device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );//开启ALPHA测试功能   
		Device->SetRenderState( D3DRS_ALPHAREF,        0x0f );//设置ALPHA测试参考值   
		Device->SetRenderState( D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL );//设置APLHA测试比较规则

		Device->SetTransform( D3DTS_WORLD,&( matScale * matRot * matBillboardRot * matWorld ) );

		Device->SetStreamSource( 0, muzzle_VB, 0, sizeof(MUZZLEVERTEX) );
		Device->SetFVF( MUZZLEVERTEX_FVF );
		Device->SetRenderState( D3DRS_LIGHTING, false );
		Device->SetTexture( 0, muzzle_tex );
		Device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
		Device->SetRenderState( D3DRS_LIGHTING, true );
		
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE,false);

	}
	
}

void MyGun::Fire()
{
	firestate = true;
}

void MyGun::Fired()
{
	firestate = false;
}