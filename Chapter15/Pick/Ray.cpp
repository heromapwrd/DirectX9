#include "Ray.h"

bool CalcPickingRay(int x, int y, IDirect3DDevice9* pDevice,Ray* pRay)
{
	if (!pDevice || !pRay)
		return false;
	float px = 0.0f;
	float py = 0.0f;
	D3DVIEWPORT9 viewport;
	pDevice->GetViewport(&viewport);
	D3DXMATRIX proj;
	pDevice->GetTransform(D3DTS_PROJECTION, &proj);

	px = ((2.0f*x) / viewport.Width - 1.0f) / proj(0,0);
	py = ((-2.0f*y) / viewport.Height + 1.0f) / proj(1, 1);

	pRay->_origin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pRay->_direction = D3DXVECTOR3(px, py, 1.0f);
	return true;
}

void TransformRay(Ray* pRay, D3DXMATRIX* pMatrix)
{
	if (!pRay || !pMatrix)
		return;
	D3DXVec3TransformCoord(&pRay->_origin, &pRay->_origin, pMatrix);
	D3DXVec3TransformNormal(&pRay->_direction, &pRay->_direction, pMatrix);
	D3DXVec3Normalize(&pRay->_direction, &pRay->_direction);
}

bool RayHitSephere(Ray* pRay, BoundingSphere* pBSephere)
{
	if (!pRay || !pBSephere)
		return false;
	D3DXVECTOR3 v = pRay->_origin - pBSephere->_center;
	float b = 2.0f*D3DXVec3Dot(&pRay->_direction, &v);
	float c = D3DXVec3Dot(&v, &v) - (pBSephere->_radius*pBSephere->_radius);

	float discriminant = (b*b) - 4.0f*c;
	if (discriminant < 0.0f)
		return false;
	discriminant = sqrtf(discriminant);
	float s0 = (-b + discriminant) / 2.0f;
	float s1 = (-b - discriminant) / 2.0f;
	
	if (s0 >= 0.0f || s1 >= 0.0f)
		return true;
	return false;
}