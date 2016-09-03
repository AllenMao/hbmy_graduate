#pragma once

#include"Common.h"

namespace PSys
{
	struct Particle
	{
		D3DXVECTOR3 vPosition;
		DWORD	dwColor;
		//float	fSize;
		static const DWORD FVF;
	};

	struct Attribute
	{
		Attribute()
		{
			fLifeTime = 0.0f;
			fAge      = 0.0f;
			bIsAlive  = true;
		}

		D3DXVECTOR3 vPosition;
		D3DXVECTOR3 vVelocity;//speed
		D3DXVECTOR3 vAcceleration;//a
		float fLifeTime;
		float fAge;
		D3DXCOLOR colorInit;// current color of the particle 
		D3DXCOLOR colorFade;//ÀÊ ±º‰ºı»ı
		bool bIsAlive;
	};

	//collide area
	struct BoundingBox
	{
		BoundingBox();

		bool isPointInside(D3DXVECTOR3& p);

		D3DXVECTOR3 vMin;
		D3DXVECTOR3 vMax;
	};

	struct BoundingSphere
	{
		BoundingSphere();

		D3DXVECTOR3 vCenter;
		float       fRadius;
	};
	
	//Define PSystem class
	class PSystem
	{
	public:
		PSystem();
		virtual ~PSystem();

		virtual bool init(IDirect3DDevice9* device, char* texFileName);
		virtual void reset();

		virtual void resetParticle(Attribute* attribute) = 0;
		virtual void addParticle();
		
		virtual void update(float timeDelta) = 0;

		virtual void preRender();
		virtual void render();
		virtual void postRender();

		bool isEmpty();
		bool isDead();

		//extra
		float GetRandomFloat(float lowBound, float highBound);

		void GetRandomVector(D3DXVECTOR3* out,D3DXVECTOR3* min,
							D3DXVECTOR3* max);

		DWORD FtoDw(float value);

	protected:
		virtual void removeDeadParticles();

	protected:
		IDirect3DDevice9*	m_pDirect3DDevice;
		D3DXVECTOR3			m_vOrigin;
		BoundingBox			m_boundingBox;
		float				m_fEmitRate;
		float				m_fSize;
		IDirect3DTexture9*  m_pTex;
		IDirect3DVertexBuffer9* m_pVb;
		std::list<Attribute> m_lParticles;
		int					m_nMaxParticles;

		DWORD				m_dwVbSize;
		DWORD				m_dwVbOffset;
		DWORD				m_dwVbBatchSize;
	};

	//define Snow
	class Snow : public PSystem
	{
	public:
		Snow(BoundingBox* boundingBox, int numParticles);
		void resetParticle(Attribute* attribute);
		void update(float timeDelta);
	};

	class Fire : public PSystem
	{
	public:
		Fire(D3DXVECTOR3* origin, int numParticles);

		void resetParticle(Attribute* attribute);
		void update(float timeDelta);
		void preRender();
		void postRender();
	};
}