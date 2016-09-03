#include "Enemy.h"


Enemy::Enemy(IDirect3DDevice9 *Device)
{
	enemy = new CSkinMesh(Device);

}

Enemy::~Enemy(void)
{
}


void Enemy::Init(IDirect3DDevice9 *Device,CTerrain *Terrain)
{
	enemy->LoadFromXFile("res/model/girl.x");
	angle = 2*D3DX_PI*(rand()%1000)/1000.0f;
	state = 0;
	HP = 48;
	pos = D3DXVECTOR3(500,Terrain->GetHeight(500,500),500);
	angle = 0;
	walkspeed = 0.8f;
	Scale =5.5f;
	enemy->m_fScale = Scale;
	enemy->m_vPos =pos;
	m_Max=D3DXVECTOR3(10.0f, 90.0f, 30.0f);
    m_Min=D3DXVECTOR3(-30.0f, 0.0f, -30.0f);
}

void Enemy::Move(CCamera *Camera,CTerrain *Terrain)
{
	if(state !=2){
		D3DXVECTOR3 campos;
		Camera->GetPos(&campos);
		D3DXVECTOR3 target = campos - pos;
		angle = atanf(target.x / target.z);	
		if (target.z < 0)
		{
			angle += D3DX_PI/2;
		}
		else
		{
			angle -= D3DX_PI/2;
		}
		float Xdiff = (float)fabs(campos.x- pos.x);
		float Zdiff = (float)fabs(campos.z- pos.z);
		distance = sqrt(Xdiff*Xdiff + Zdiff*Zdiff);//¾àÀë

		float MoveX = (campos.x-pos.x-50)/distance *walkspeed;
		float MoveZ = (campos.z -pos.z-50)/distance *walkspeed;
		pos.x += MoveX;
		pos.z += MoveZ;
		pos.y = Terrain->GetCameraHeight(pos.x,pos.z);
		min = m_Min +pos;
		max = m_Max +pos;
	}
}

void Enemy::Render(IDirect3DDevice9 *Device,float time)
{
	
	if(state !=2){
	if(distance >100){
		Run();
		enemy->Render(3*time,pos,angle+1.5,Scale);
	}
	else
	{
		Attack();
		enemy->Render(6*time,pos,angle+1.5,Scale);//ËÙ¶ÈÂÔ¿ì
	}
	}
	enemy->Render(2.7*time,pos,angle+1.5,Scale);
}

void Enemy::Attack()
{
	enemy->SetAnimationName("fight");
	state =1;
}

void Enemy::Run()
{
	enemy->SetAnimationName("run");
	state = 0;
}

void Enemy::Hurt()
{
	HP -=1;
	if(HP<0)
		HP=0;
}

void Enemy::Die()
{
	enemy->SetAnimationName("fight_down");
	state =2;
}

void Enemy::Stop()
{
	enemy->SetAnim(false);
}


int Enemy::GetHP()
{
	return HP;
}
void Enemy::SetHP(int hp)
{
	HP = hp;
}

int Enemy::GetState()
{
	return state;
}

int Enemy::GetDistance()
{
	return distance;
}

D3DXVECTOR3 Enemy::GetPos()
{
	return pos;
}

D3DXVECTOR3 Enemy::GetMin()
{
	return min;
}

D3DXVECTOR3 Enemy::GetMax()
{
	return max;
}