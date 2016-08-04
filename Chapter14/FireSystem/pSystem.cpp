#include "pSystem.h"
#include<cstdlib>
using namespace psys;


PSystem::PSystem()
{
	m_pDevice = NULL;
	m_pVB = NULL;
	m_pTex = NULL;
}

PSystem::~PSystem()
{
	d3d9::Release(m_pVB);
	d3d9::Release(m_pTex);
}

bool PSystem::Init(IDirect3DDevice9* pDevice, const char* filename)
{
	if (!pDevice || !filename)
		return false;
	m_pDevice = pDevice;

	HRESULT hr = 0;
	hr = m_pDevice->CreateVertexBuffer(
		m_vbSize*sizeof(Particle),
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS,
		Particle::FVF, D3DPOOL_DEFAULT,
		&m_pVB,
		NULL
		);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create vertex buff!", 0, 0);
		return false;
	}

	hr = D3DXCreateTextureFromFile(
		m_pDevice,
		filename,
		&m_pTex
		);
	if (FAILED(hr))
	{
		MessageBox(0, "Failed to create texture!", 0, 0);
		return false;
	}
	return true;
}

void PSystem::Reset()
{
	list<Attribute>::iterator it;
	for (it = m_lParticles.begin(); it != m_lParticles.end(); it++)
	{
		ResetParticle(&(*it));
	}
}

void PSystem::AddParticle()
{
	Attribute attribute;
	ResetParticle(&attribute);
	m_lParticles.push_back(attribute);
}

void PSystem::PreRender()
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING,false);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, d3d9::FtoDw(m_Size));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, d3d9::FtoDw(0.0f));

	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, d3d9::FtoDw(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, d3d9::FtoDw(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, d3d9::FtoDw(1.0f));

	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void PSystem::Render()
{
	if (!IsEmpty())
	{
		PreRender();
		m_pDevice->SetTexture(0, m_pTex);
		m_pDevice->SetFVF(Particle::FVF);
		m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Particle));

		if (m_vbOffset >= m_vbSize)
			m_vbOffset = 0;
		
		Particle* pParticles;
		m_pVB->Lock(
			m_vbOffset*sizeof(Particle),
			m_vbBatchSize*sizeof(Particle),
			(void**)&pParticles,
			m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD
			);
		DWORD dwNumParticlesInBatch = 0;
		list<Attribute>::iterator it = m_lParticles.begin();
		while (it != m_lParticles.end())
		{
			if (!it->_isAlive)
			{
				it++;
				continue;
			}
			pParticles[dwNumParticlesInBatch]._pos = it->_pos;
			pParticles[dwNumParticlesInBatch]._color = it->_color;
			dwNumParticlesInBatch++;
			
			if (dwNumParticlesInBatch>=m_vbBatchSize)
			{
				m_pVB->Unlock();
				m_pDevice->DrawPrimitive(
					D3DPT_POINTLIST,
					m_vbOffset,
					m_vbBatchSize
					);

				m_vbOffset += m_vbBatchSize;
				if (m_vbOffset >= m_vbSize)
					m_vbOffset = 0;

				m_pVB->Lock(
					m_vbOffset*sizeof(Particle), 
					m_vbBatchSize*sizeof(Particle), 
					(void**)&pParticles,
					m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD
					);
				dwNumParticlesInBatch = 0;
			}
			it++;
		}

		m_pVB->Unlock();

		if (dwNumParticlesInBatch)
		{
			m_pDevice->DrawPrimitive(
				D3DPT_POINTLIST,
				m_vbOffset,
				dwNumParticlesInBatch
				);
			m_vbOffset += m_vbBatchSize;
		}

		PostRender();
	}
}

void PSystem::PostRender()
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, true);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

bool PSystem::IsEmpty()
{
	return m_lParticles.empty();
}

bool PSystem::IsDead()
{
	list<Attribute>::iterator it;
	for (it = m_lParticles.begin(); it != m_lParticles.end(); it++)
	{
		if (it->_isAlive)
			return false;
	}
	return true;
}

void PSystem::RemoveDeadedParticles()
{
	list<Attribute>::iterator it;
	it = m_lParticles.begin();
	while (it != m_lParticles.end())
	{
		if (it->_isAlive)
		{
			it++;
		}
		else
		{
			it = m_lParticles.erase(it);
		}
	}
}


