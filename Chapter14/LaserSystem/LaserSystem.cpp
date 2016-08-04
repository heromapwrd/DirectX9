#include "LaserSystem.h"

Laser::Laser(Camera* pCamera)
{
	m_pCamera = pCamera;
	m_Size = 0.8f;
	m_vbSize = 2048;
	m_vbOffset = 0;
	m_vbBatchSize = 512;
}

void Laser::ResetParticle(Attribute* pAttribute)
{
	if (!pAttribute||!m_pCamera)
		return;

	pAttribute->_isAlive = true;

	D3DXVECTOR3 camerapos;
	m_pCamera->GetPos(&camerapos);
	pAttribute->_pos = camerapos;
	pAttribute->_pos.y = camerapos.y - 1;

	pAttribute->_lifetime = 1.0f;
	pAttribute->_age = 0.0f;
	pAttribute->_color = d3d9::GREEN_COLOR;

	D3DXVECTOR3 cameradir;
	m_pCamera->GetLook(&cameradir);
	pAttribute->_velocity = cameradir * 50;

}

void Laser::Update(float timeDelta)
{
	list<Attribute>::iterator it = m_lParticles.begin();
	while (it != m_lParticles.end())
	{
		it->_pos += timeDelta*it->_velocity;
		it->_age += timeDelta;
		if (it->_age>it->_lifetime)
		{
			it->_isAlive = false;
		}
		it++;
	}
	RemoveDeadedParticles();
}
