#ifndef _FIRESYSTEM_H__
#define _FIRESYSTEM_H__
#include "pSystem.h"
using namespace psys;

class FireWork:public PSystem
{
public:
	FireWork(D3DXVECTOR3* pOrigin, int iNumParticles);
protected:
	virtual void ResetParticle(Attribute* pAttribute) override;
	virtual void Update(float timeDelta) override;
	virtual void PreRender() override;
	virtual void PostRender() override;
};

#endif