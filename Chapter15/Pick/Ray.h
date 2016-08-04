#ifndef _RAY_H__
#define _RAY_H__
#include "Bound.h"

struct Ray
{
	D3DXVECTOR3 _origin;
	D3DXVECTOR3 _direction;
};

bool CalcPickingRay(int x, int y, IDirect3DDevice9* pDevice,Ray* pRay);
void TransformRay(Ray* pRay, D3DXMATRIX* pMatrix);
bool RayHitSephere(Ray* pRay, BoundingSphere* pBSephere);

#endif