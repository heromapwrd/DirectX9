#include "FireSystem.h"

FireWork::FireWork(D3DXVECTOR3* pOrigin, int iNumParticles)
{
	m_Origin = *pOrigin;
	m_Size = 0.9f;
	m_vbSize = 2048;
	m_vbOffset = 0;
	m_vbBatchSize = 512;

	for (int i = 0; i < iNumParticles; i++)
		AddParticle();
}

void FireWork::ResetParticle(Attribute* pAttribute)
{
	pAttribute->_isAlive = true;
	pAttribute->_pos = m_Origin;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	d3d9::GetRandomVector(
		&pAttribute->_velocity,
		&min,
		&max
		);
	D3DXVec3Normalize(
		&pAttribute->_velocity,
		&pAttribute->_velocity
		);
	pAttribute->_velocity *= 100.0f;
	pAttribute->_color = D3DXCOLOR(
		d3d9::GetRandomFloat(0.0f, 1.0f),
		d3d9::GetRandomFloat(0.0f, 1.0f),
		d3d9::GetRandomFloat(0.0f, 1.0f),
		1.0f
		);
	pAttribute->_age = 0.0f;
	pAttribute->_lifetime = 2.0f;

}

void FireWork::Update(float timeDelta)
{
	list<Attribute>::iterator it = m_lParticles.begin();
	while (it != m_lParticles.end())
	{
		if (it->_isAlive)
		{
			it->_pos += timeDelta*it->_velocity;
			it->_age += timeDelta;
			if (it->_age > it->_lifetime)
				it->_isAlive = false;
		}
		it++;
	}
}

void FireWork::PreRender()
{
	PSystem::PreRender();
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

}

void FireWork::PostRender()
{
	PSystem::PostRender();
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
}
