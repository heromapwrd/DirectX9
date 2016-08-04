#include"Bound.h"
#include "d3dUtility.h"
using namespace d3d9;

BoundingBox::BoundingBox()
{
	_min.x = _INFINITY;
	_min.y = _INFINITY;
	_min.z = _INFINITY;
	_max.x = -_INFINITY;
	_max.y = -_INFINITY;
	_max.z = -_INFINITY;
}

bool BoundingBox::isPointInside(D3DXVECTOR3& p)
{
	if (
		p.x<_max.x&&p.x>_min.x&&
		p.y<_max.y&&p.y>_min.y&&
		p.z<_max.z&&p.z>_min.z
		)
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
	_radius = 0.0f;
}