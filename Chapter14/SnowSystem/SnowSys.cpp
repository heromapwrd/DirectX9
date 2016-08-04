#include "SnowSys.h"

Snow::Snow(BoundingBox* pBox, int iNumParticles)
{
	m_BBox = *pBox;
	m_Size = 0.25f;
	m_vbSize = 2048;
	m_vbOffset = 0;
	m_vbBatchSize = 512;
	for (int i = 0; i < iNumParticles;i++)
	{
		AddParticle();
	}
}

void Snow::ResetParticle(Attribute* pAttribute)
{
	pAttribute->_isAlive = true;
	d3d9::GetRandomVector(
		&pAttribute->_pos,
		&m_BBox._min,
		&m_BBox._max
		);
	pAttribute->_pos.y = m_BBox._max.y;

	pAttribute->_velocity.z = 0;
	pAttribute->_velocity.x = (d3d9::GetRandomFloat(0.0f, 2.0f)-1)*3.0f;
	pAttribute->_velocity.y = d3d9::GetRandomFloat(0.0f, 1.0f)*-10.0f;

	pAttribute->_color = d3d9::WHITE_COLOR;
}

void Snow::Update(float timeDelta)
{
	list<Attribute>::iterator it = m_lParticles.begin();
	while (it != m_lParticles.end())
	{
		it->_pos += timeDelta*it->_velocity;
		if (!m_BBox.isPointInside(it->_pos))
		{
			ResetParticle(&(*it));
		}
		it++;
	}
}

