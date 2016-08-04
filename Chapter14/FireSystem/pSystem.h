#ifndef _PSYSTEM_H__
#define _PSYSTEM_H__
#include "d3dUtility.h"
#include "Camera.h"
#include "Bound.h"
#include<list>
using std::list;

namespace psys
{
	struct Particle
	{
		D3DXVECTOR3 _pos;
		D3DCOLOR _color;
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	};
	struct Attribute 
	{
		Attribute()
		{
			_lifetime = 0.0f;
			_age = 0.0f;
			_isAlive = true;
		}
		D3DXVECTOR3 _pos;
		D3DXVECTOR3 _velocity;
		D3DXVECTOR3 _acceleration;
		float _lifetime;
		float _age;
		D3DXCOLOR _color;
		D3DXCOLOR _colorFade;
		bool _isAlive;
	};
	class PSystem
	{
	public:
		PSystem();
		virtual ~PSystem();

		virtual bool Init(IDirect3DDevice9* pDevice, const char* filename);
		virtual void Reset();

		virtual void ResetParticle(Attribute* pAttribute)=0;
		virtual void AddParticle();

		virtual void Update(float timeDelta) = 0;
		virtual void PreRender();
		virtual void Render();
		virtual void PostRender();

		bool IsEmpty();
		bool IsDead();

	protected:
		virtual void RemoveDeadedParticles();
	protected:
		IDirect3DDevice9* m_pDevice;
		IDirect3DVertexBuffer9* m_pVB;
		IDirect3DTexture9* m_pTex;
		D3DXVECTOR3	m_Origin;
		BoundingBox m_BBox;
		float m_AddRate;
		float m_Size;
		list<Attribute> m_lParticles;
		int m_MaxParticlesNum;

		DWORD m_vbSize;
		DWORD m_vbOffset;
		DWORD m_vbBatchSize;
	};
}

#endif