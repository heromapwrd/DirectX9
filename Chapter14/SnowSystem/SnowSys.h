#ifndef _SNOWSYS_H__
#define _SNOWSYS_H__
#include "pSystem.h"
using namespace psys;
class Snow:public PSystem
{
public:
	Snow(BoundingBox* pBox, int iNumParticles);
protected:
	virtual void ResetParticle(Attribute* pAttribute) override;
	virtual void Update(float timeDelta) override;

};

#endif