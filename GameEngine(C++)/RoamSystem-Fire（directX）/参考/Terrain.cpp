#include "Terrain.h"

CTerrain::CTerrain(int r, int c, float length)
{
	this->length = length;
	this->num_col = c;
	this->num_row = r;
	this->uTexSize = 1.0f / (float)(this->num_col-1);
	this->vTexSize = 1.0f / (float)(this->num_row-1);	
}

CTerrain::~CTerrain(void)
{
	m_pVertexBuffer->Release();
	m_pIndexBuffer->Release();
}

float CTerrain::GetCameraHeight(float x, float z)
{
	//将地图移动到原点,方便计算
	x = ((float)(num_col-1)*length / 2.0f) + x;
	z = ((float)(num_row-1)*length / 2.0f) - z;
	//将地图大小单位化，方便计算
	x /= (float)length;
	z /= (float)length;
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

bool CTerrain::InitTerrain(IDirect3DDevice9 *Device, LPCSTR rawFileName, LPCSTR texFilename,LPCSTR texFilename1,float heightScale, bool OneTex)
{
	BYTE *hightmap = new BYTE[num_row*num_col];
	FILE *fp =  fopen(rawFileName, "rb");
	ZeroMemory ( hightmap , sizeof(BYTE)*num_col*num_row );
	fread( hightmap , 1, num_row*num_col, fp );
	fclose(fp);

	raw.resize((num_col)*(num_row));
	for(size_t i=0; i<(size_t)num_col*num_row; i++)
	{
		raw[i] = hightmap[i];
		raw[i] *= heightScale;
	}
	delete [] hightmap;
	WORD *v;
	TERRAINVERTEX *vertex;
	if(FAILED(Device->CreateVertexBuffer(num_row * num_col * sizeof(TERRAINVERTEX), 0, TERRAINVERTEX_FVF, D3DPOOL_MANAGED,&m_pVertexBuffer, 0)))
	{
		return false;
	}
	this->m_pVertexBuffer->Lock(0, 0, (void**)&vertex, 0);
	int k = 0;
	if(OneTex)
	{
		for(int i=0; i<this->num_row; i++)
		{
			for(int j=0; j<this->num_col; j++)
			{
				vertex[i*num_col + j] = TERRAINVERTEX((float)((-num_col/2+j)*length), raw[k], float((num_row/2-i)*length), 0.0f, 0.0f, 0.0f, (float)j*this->uTexSize, (float)i*this->vTexSize,(float)j*this->uTexSize, (float)i*this->vTexSize);
				k++;
			}
		}
	}
	else
	{
		for(int i=0; i<this->num_col; i++)
		{
			for(int j=0; j<this->num_row; j++)
			{
				vertex[i*num_col + j] = TERRAINVERTEX((float)((-num_col/2+j)*length), raw[k], float((num_row/2-i)*length), 0.0f, 0.0f, 0.0f, (float)(j%2), (float)(i%2),(float)(j%2), (float)(i%2));
				k++;
			}
		}
	}
	this->m_pVertexBuffer->Unlock();

	if(FAILED(Device->CreateIndexBuffer((this->num_col-1) * (this->num_row-1) * 6 * sizeof(WORD), 0, D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_pIndexBuffer,NULL)))
	{
		return false;
	}
	if(FAILED(m_pIndexBuffer->Lock(0, 0, (void**)&v, 0)))
	{
		return false;
	}
	this->m_pVertexBuffer->Lock(0, 0, (void**)&vertex, 0);
	int index = 0;
	//计算索引以及法向量
	for(int i=0; i<(this->num_col-1); i++)
	{
		for(int j=0; j<(this->num_row-1); j++)
		{
			v[index] = i * this->num_row + j;
			v[index+1] = i * this->num_row + j + 1;
			v[index+2] = (i+1) * this->num_row + j;

			D3DVECTOR temp;
			ComputeTriNormal(vertex[v[index]], vertex[v[index+1]], vertex[v[index+2]], temp);
			vertex[v[index]].nx = temp.x;
			vertex[v[index]].ny = temp.y;
			vertex[v[index]].nz = temp.z;

			vertex[v[index+1]].nx = temp.x;
			vertex[v[index+1]].ny = temp.y;
			vertex[v[index+1]].nz = temp.z;

			vertex[v[index+2]].nx = temp.x;
			vertex[v[index+2]].ny = temp.y;
			vertex[v[index+2]].nz = temp.z;

			v[index+3] = (i+1) * this->num_row + j;
			v[index+4] = i * this->num_row + j + 1;
			v[index+5] = (i+1) * this->num_row + j + 1;

			ComputeTriNormal(vertex[v[index+3]], vertex[v[index+4]], vertex[v[index+5]], temp);

			vertex[v[index+3]].nx = temp.x;
			vertex[v[index+3]].ny = temp.y;
			vertex[v[index+3]].nz = temp.z;

			vertex[v[index+4]].nx = temp.x;
			vertex[v[index+4]].ny = temp.y;
			vertex[v[index+4]].nz = temp.z;

			vertex[v[index+5]].nx = temp.x;
			vertex[v[index+5]].ny = temp.y;
			vertex[v[index+5]].nz = temp.z;

			index += 6;
		}
	}
	this->m_pVertexBuffer->Unlock();
	m_pIndexBuffer->Unlock();
	if(FAILED(D3DXCreateTextureFromFile(Device,_T(texFilename),&Dtex)))
	{
		return false;
	}
	if(FAILED(D3DXCreateTextureFromFile(Device,_T(texFilename1),&Dtex1)))
	{
		return false;
	}
	return true;
}

void CTerrain::Render(IDirect3DDevice9 *Device)
{
	Device->SetTexture(0,Dtex);
	Device->SetTexture(1,Dtex1);
	Device->SetStreamSource(0,m_pVertexBuffer,0,sizeof(TERRAINVERTEX));
	Device->SetIndices(m_pIndexBuffer);
	Device->SetFVF(TERRAINVERTEX_FVF);
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);
	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,this->num_col*this->num_row,0,(this->num_col-1)*(this->num_row-1)*2);	
}

float CTerrain::GetHeight(int r, int l)
{
	float y;
	if(r >= 0 && l >= 0 && r <= this->num_row && l <= num_col)
	{
		y =  (float)raw[r*this->num_col + l];
	}
	else
	{
		y = 0;
	}
	return y;
}

void CTerrain::ComputeTriNormal(TERRAINVERTEX &v1, TERRAINVERTEX &v2, TERRAINVERTEX &v3, D3DVECTOR &normal)
{
	D3DXVECTOR3 vec1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
	D3DXVECTOR3 vec2(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
	D3DXVECTOR3 temp;
	D3DXVec3Cross(&temp, &vec1, &vec2);
	D3DXVec3Normalize(&temp, &temp);
	normal = (D3DVECTOR)temp;
}