#pragma once
#include "particles.h"
#include"Common.h"

using namespace PSys;

const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
//collide function
BoundingBox::BoundingBox()
{
	// infinite small 
	vMin.x = FLT_MAX;
	vMin.y = FLT_MAX;
	vMin.z = FLT_MAX;

	vMax.x = FLT_MAX;
	vMax.y = FLT_MAX;
	vMax.z = FLT_MAX;
}

bool BoundingBox::isPointInside(D3DXVECTOR3& p)
{
	if( p.x >= vMin.x && p.y >= vMin.y && p.z >= vMin.z &&
		p.x <= vMax.x && p.y <= vMax.y && p.z <= vMax.z )
	{
		return true;
	}
	else
	{
		return false;
	}
}

BoundingSphere::BoundingSphere()
{
	fRadius = 0.0f;
}

//Randomness
float PSystem::GetRandomFloat(float lowBound, float highBound)
{
	if( lowBound >= highBound ) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f; 

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound; 
}

void PSystem::GetRandomVector(
	D3DXVECTOR3* out,
	D3DXVECTOR3* min,
	D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

DWORD PSystem::FtoDw(float value)
{
	return *((DWORD*)&value);
}

PSystem::PSystem()
{
	m_pDirect3DDevice = 0;
	m_pVb     = 0;
	m_pTex    = 0;
}

PSystem::~PSystem()
{
	m_pVb->Release();
	m_pTex->Release();
}

bool PSystem::init(IDirect3DDevice9* device, char* texFileName)
{
	/*vertex buffer's size does not equal the number of particles in our system.  We
	use the vertex buffer to draw a portion of our particles at a time.  The arbitrary
	size we choose for the vertex buffer is specified by the m_fVbSize variable.*/
	m_pDirect3DDevice = device; // save a ptr to the device

	HRESULT hr = 0;

	hr = device->CreateVertexBuffer(
		m_dwVbSize * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		Particle::FVF,
		D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
		&m_pVb,
		0);

	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexBuffer() - FAILED", "PSystem", 0);
		return false;
	}

	hr = D3DXCreateTextureFromFile(
		device,
		texFileName,
		&m_pTex);

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateTextureFromFile() - FAILED", "PSystem", 0);
		return false;
	}

	return true;

}

void PSystem::reset()
{
	std::list<Attribute>::iterator i;
	for(i = m_lParticles.begin(); i != m_lParticles.end(); ++i)
	{
		resetParticle(&(*i));
	}
}

void PSystem::addParticle()
{
	Attribute attribute;
	resetParticle(&attribute);
	m_lParticles.push_back(attribute);
}

void PSystem::preRender()
{
	m_pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_pDirect3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pDirect3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true); 
	m_pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE, FtoDw(m_fSize));
	m_pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

	// control the size of the particle relative to distance
	m_pDirect3DDevice->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	m_pDirect3DDevice->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	m_pDirect3DDevice->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

	// use alpha from texture
	m_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void PSystem::postRender()
{
	m_pDirect3DDevice->SetRenderState(D3DRS_LIGHTING,          true);
	m_pDirect3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pDirect3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE,  false);
	m_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,  false);
}

void PSystem::render()
{
	if(!m_lParticles.empty())
	{
		preRender();

		m_pDirect3DDevice->SetTexture(0, m_pTex);
		m_pDirect3DDevice->SetFVF(Particle::FVF);
		m_pDirect3DDevice->SetStreamSource(0, m_pVb, 0, sizeof(Particle));

		//start at beginning if we're at end of the m_pVb
		if(m_dwVbOffset >= m_dwVbSize)
		{
			m_dwVbOffset = 0;
		}
		Particle* v = 0;

		m_pVb->Lock(m_dwVbOffset * sizeof(Particle),
			m_dwVbBatchSize * sizeof(Particle),
			(void**)&v,
			m_dwVbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		//until all particle have been rendered
		std::list<Attribute>::iterator i;
		for(i = m_lParticles.begin(); i != m_lParticles.end(); i++)
		{
			if(i->bIsAlive)
			{
				/* Copy a batch of the living particles to the
				next vertex buffer segment*/
				v->vPosition = i->vPosition;
				v->dwColor = (D3DCOLOR)i->colorInit;
				v++;//next

				++numParticlesInBatch;

				//if this batch full!
				if(numParticlesInBatch == m_dwVbBatchSize)
				{
					/* Draw the last batch of particles that was
					copied to the vertex buffer.*/
					m_pVb->Unlock();

					m_pDirect3DDevice->DrawPrimitive(
						D3DPT_POINTLIST,
						m_dwVbOffset,
						m_dwVbBatchSize);

					//
					// While that batch is drawing, start filling the
					// next batch with particles.
					//

					// move the offset to the start of the next batch
					m_dwVbOffset += m_dwVbBatchSize; 

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					if(m_dwVbOffset >= m_dwVbSize) 
						m_dwVbOffset = 0;       

					m_pVb->Lock(
						m_dwVbOffset    * sizeof( Particle ),
						m_dwVbBatchSize * sizeof( Particle ),
						(void**)&v,
						m_dwVbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch
				}
			}
		}

		m_pVb->Unlock();

		/* its possible that the LAST batch being filled never 
		got rendered because the condition 
		(numParticlesInBatch == _vbBatchSize) would not have
		been satisfied.  We draw the last partially filled batch now.*/

		if(numParticlesInBatch)
		{
			m_pDirect3DDevice->DrawPrimitive(D3DPT_POINTLIST,
				m_dwVbOffset,
				numParticlesInBatch);
		}

		m_dwVbOffset += m_dwVbBatchSize;

		//reset render states
		postRender();

	}
}

bool PSystem::isEmpty()
{
	return m_lParticles.empty();
}

bool PSystem::isDead()
{
	std::list<Attribute>::iterator i;
	for(i = m_lParticles.begin(); i != m_lParticles.end(); i++)
	{
		// is there at least one living particle?  If yes,
		// the system is not dead.
		if( i->bIsAlive )
			return false;
	}
	// no living particles found, the system must be dead.
	return true;
}

void PSystem::removeDeadParticles()
{
	std::list<Attribute>::iterator i;

	i = m_lParticles.begin();

	while( i != m_lParticles.end() )
	{
		if( i->bIsAlive == false )
		{
			// erase returns the next iterator, so no need to
			// incrememnt to the next one ourselves.
			i = m_lParticles.erase(i); 
		}
		else
		{
			i++; // next in list
		}
	}
}

//***********************************Snow*******************************//
Snow::Snow(BoundingBox* boundingBox, int numParticles)
{
	m_boundingBox = *boundingBox;
	m_fSize = 5.0f;
	m_dwVbSize = 2048;
	m_dwVbOffset = 0;
	m_dwVbBatchSize = 512;

	for(int i = 0; i < numParticles; ++i)
		addParticle();
}

void Snow::resetParticle(Attribute* attribute)
{
	attribute->bIsAlive = true;

	//get rendom x,z coordinate for position of the snow flake
	GetRandomVector(&attribute->vPosition,
		&m_boundingBox.vMin,
		&m_boundingBox.vMax);

	//no randomness for y
	//attribute->vPosition.y = m_boundingBox.vMax.y;

	attribute->vVelocity.x = GetRandomFloat(0.0f, 1.0f) * (-30.0f);
	attribute->vVelocity.y = GetRandomFloat(0.0f, 1.0f) * (-100.0f);
	attribute->vVelocity.z = 0.0f;

	attribute->colorInit = D3DCOLOR_XRGB(255, 255, 255);
}

void Snow::update(float timeDelta)
{
	std::list<Attribute>::iterator i;
	for(i = m_lParticles.begin(); i != m_lParticles.end(); i++)
	{
		i->vPosition += i->vVelocity * timeDelta;
		if(m_boundingBox.isPointInside(i->vPosition) == false)
		{
			resetParticle(&(*i));
		}
	}

}
//////////////////////Fire/////////////////////////
Fire::Fire(D3DXVECTOR3* origin, int numParticles)
{
	m_vOrigin        = *origin;
	m_fSize          = 0.9f;
	m_dwVbSize        = 2048;
	m_dwVbOffset      = 0;   
	m_dwVbBatchSize   = 512; 

	for(int i = 0; i < numParticles; i++)
		addParticle();
}

void Fire::resetParticle(Attribute* attribute)
{
	attribute->bIsAlive  = true;
	attribute->vPosition = m_vOrigin;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);

	GetRandomVector(
		&attribute->vVelocity,
		&min,
		&max);

	// normalize to make spherical
	D3DXVec3Normalize(
		&attribute->vVelocity,
		&attribute->vVelocity);

	attribute->vVelocity *= 20.0f;

	attribute->colorInit = D3DXCOLOR(
		GetRandomFloat(0.0f, 1.0f),
		GetRandomFloat(0.0f, 1.0f),
		GetRandomFloat(0.0f, 1.0f),
		1.0f);

	attribute->fAge      = 0.0f;
	attribute->fLifeTime = 2.0f; // lives for 2 seconds
}

void Fire::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i = m_lParticles.begin(); i != m_lParticles.end(); i++)
	{
		// only update living particles
		if( i->bIsAlive )
		{
			i->vPosition += i->vVelocity * timeDelta;

			i->fAge += timeDelta;

			if(i->fAge > i->fLifeTime) // kill 
				i->bIsAlive = false;
		}
	}
}

void Fire::preRender()
{
	PSystem::preRender();

	m_pDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    m_pDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// read, but don't write particles to z-buffer
	m_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void Fire::postRender()
{
	PSystem::postRender();

	m_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
}
