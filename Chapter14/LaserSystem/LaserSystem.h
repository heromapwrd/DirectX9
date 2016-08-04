#ifndef _LASERSYSTEM_H__
#define _LASERSYSTEM_H__
#include "pSystem.h"
using namespace psys;
class Laser:public PSystem
{
public:
	Laser(Camera* pCamera);
protected:
	virtual void ResetParticle(Attribute* pAttribute) override;
	virtual void Update(float timeDelta) override;
protected:
	Camera* m_pCamera;

};

#endif