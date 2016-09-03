#include "Item.h"

Item::Item(IDirect3DDevice9 *Device)
{
	item = new CSkinMesh(Device);
}

Item::~Item()
{
}

void Item::Init(IDirect3DDevice9 *Device)
{
	item->LoadFromXFile("res/model/firstaid.x");
	angle = 2*D3DX_PI*(rand()%1000)/1000.0f;//随机方向
	pos =D3DXVECTOR3 (float((rand()%5000 -2500)),float((rand()%55 +30)),float((rand()%4000 - 2000)));
	Scale =(rand()%10)*1.0f;
	effect =true;
}

void Item::Update(CCamera *Camera,float time)
{
	CheckDistance(Camera);
	if(state ==0)
		item->SetDirection(angle + time*2000);
	if(distance<50)
	{
		state = 1;
	}
}

void Item::Render(IDirect3DDevice9 *Device,float time)
{
	if(state != 1)//正常状态下，渲染，否则不渲染
		item->Render(time,pos,angle,Scale);
	angle +=1.3f*time;
}

void Item::CheckDistance(CCamera *Camera)
{
		D3DXVECTOR3 campos;
		Camera->GetPos(&campos);
		float Xdiff = (float)fabs(campos.x- pos.x);
		float Zdiff = (float)fabs(campos.z- pos.z);
		distance = sqrt(Xdiff*Xdiff + Zdiff*Zdiff);//距离
		if(distance<10*Scale)
		{
			state=1;
			effect =false;
		}
}

int Item::GetState()
{
	return state;
}

float Item::GetScale()
{
	return Scale;
}

bool Item::Overdue()
{
	return effect;
}