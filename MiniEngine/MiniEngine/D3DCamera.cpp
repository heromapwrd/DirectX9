#include "D3DCamera.h"

CD3DCamera::CD3DCamera() :m_eCameraType(CT_FLYFREEDOM), m_fWalkSpeed(100.0f), m_vScale(D3DXVECTOR3(1.0f,1.0f,1.0f))
{
	D3DXMatrixIdentity(&m_vScaleMatrix);
}

CD3DCamera::~CD3DCamera()
{

}

void CD3DCamera::Rotate(float angle, D3DXVECTOR3* pAxis, D3DXVECTOR3* pPoint /*= NULL*/)
{
	if (NULL == pAxis)
		return;
	D3DXVECTOR3 point;
	D3DXVECTOR3 axis;

	if (NULL == pPoint)
	{
		point = m_vLookAtPt;
	}
	else
	{
		point = *pPoint;
	}
	axis = *pAxis;
	D3DXMATRIX rotation;
	D3DXMatrixRotationAxis(&rotation, &axis, angle);
	D3DXMATRIX Translation, InvTranslation;
	D3DXMatrixTranslation(&Translation, -point.x, -point.y, -point.z);
	D3DXMatrixTranslation(&InvTranslation, point.x, point.y, point.z);
	D3DXMATRIX Trans;
	D3DXMatrixMultiply(&Trans, &Translation, &rotation);
	D3DXMatrixMultiply(&Trans, &Trans, &InvTranslation);
	D3DXVec3TransformCoord(&m_vEyePt, &m_vEyePt, &Trans);

	m_vViewDir = m_vLookAtPt - m_vEyePt;
	D3DXVec3TransformNormal(&m_vUpDir, &m_vUpDir, &rotation);
	D3DXVec3Cross(&m_vRightDir, &m_vUpDir, &m_vViewDir);
	D3DXVec3Cross(&m_vUpDir, &m_vViewDir, &m_vRightDir);

	Normalize();
	SetViewMatrix();
}

void CD3DCamera::Rotate(D3DXVECTOR3 axis, float angle)
{
	D3DXMATRIX Rotation;
	D3DXMatrixRotationAxis(&Rotation, &axis, angle);
	D3DXVec3TransformNormal(&m_vViewDir, &m_vViewDir, &Rotation);
	D3DXVec3TransformNormal(&m_vUpDir, &m_vUpDir, &Rotation);
	D3DXVec3Cross(&m_vRightDir, &m_vUpDir, &m_vViewDir);
	Normalize();
	float distance = D3DXVec3Length(&(m_vLookAtPt - m_vEyePt));
	m_vLookAtPt = m_vEyePt + distance*m_vViewDir;
	
	SetViewMatrix();
}

void CD3DCamera::Pitch(float angle)
{
	switch (m_eCameraType)
	{
	case CT_FLYFREEDOM:
		Rotate(m_vRightDir, angle);
		break;
	case CT_FIRSTPERSON:
		Rotate(m_vRightDir, angle);
		break;
	case CT_THIRDPERSON:
		break;
	default:
		break;
	}
}

void CD3DCamera::Yaw(float angle)
{
	switch (m_eCameraType)
	{
	case CT_FLYFREEDOM:
		Rotate(m_vUpDir, angle);
		break;
	case CT_FIRSTPERSON:
		Rotate(g_UpDir, angle);
		break;
	case CT_THIRDPERSON:
		break;
	default:
		break;
	}
}

void CD3DCamera::Roll(float angle)
{
	switch (m_eCameraType)
	{
	case CT_FLYFREEDOM:
		Rotate(m_vViewDir, angle);
		break;
	case CT_FIRSTPERSON:
		Rotate(m_vViewDir, angle);
		break;
	case CT_THIRDPERSON:
		break;
	default:
		break;
	}
}

void CD3DCamera::WalkView(float times)
{
	switch (m_eCameraType)
	{
	case CT_FLYFREEDOM:
	{
						  D3DXVECTOR3 way = m_fWalkSpeed*times*m_vViewDir;
						  m_vEyePt += way;
						  m_vLookAtPt += way;
						  SetViewMatrix();
						  break;
	}
	case CT_FIRSTPERSON:
	case CT_THIRDPERSON:
	{
						   D3DXVECTOR3 dir = D3DXVECTOR3(m_vViewDir.x, 0.0f, m_vViewDir.z);
						   D3DXVECTOR3 way = m_fWalkSpeed*times*dir;
						   m_vEyePt += way;
						   m_vLookAtPt += way;
						   SetViewMatrix();
						   break;
	}
	default:
		break;
	}
}

void CD3DCamera::WalkRight(float times)
{
	switch (m_eCameraType)
	{
	case CT_FLYFREEDOM:
	{
						  D3DXVECTOR3 way = m_fWalkSpeed*times*m_vRightDir;
						  m_vEyePt += way;
						  m_vLookAtPt += way;
						  SetViewMatrix();
						  break;
	}
	case CT_FIRSTPERSON:
	case CT_THIRDPERSON:
	{
						   D3DXVECTOR3 dir = D3DXVECTOR3(m_vRightDir.x, 0.0f, m_vRightDir.z);
						   D3DXVECTOR3 way = m_fWalkSpeed*times*dir;
						   m_vEyePt += way;
						   m_vLookAtPt += way;
						   SetViewMatrix();
						   break;
	}
	default:
		break;
	}
}

void CD3DCamera::FlayUP(float times)
{
	switch (m_eCameraType)
	{
	case CT_FLYFREEDOM:
	{
						  D3DXVECTOR3 way = m_fWalkSpeed*times*m_vUpDir;
						  m_vEyePt += way;
						  m_vLookAtPt += way;
						  SetViewMatrix();
						  break;
						  break;
	}
	case CT_FIRSTPERSON:
	case CT_THIRDPERSON:
	{
						   D3DXVECTOR3 dir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
						   D3DXVECTOR3 way = m_fWalkSpeed*times*dir;
						   m_vEyePt += way;
						   m_vLookAtPt += way;
						   SetViewMatrix();
						   break;
	}
	default:
		break;
	}
}

void CD3DCamera::SetWalkSpeed(float fSpeed)
{
	m_fWalkSpeed = fSpeed;
}

void CD3DCamera::SetScale(D3DXVECTOR3& vVecScale)
{
	D3DXMatrixScaling(&m_vScaleMatrix, 1.0f/m_vScale.x, 1.0f/m_vScale.y, 1.0f/m_vScale.z);
	D3DXMatrixMultiply(&m_vViewMatrix, &m_vViewMatrix, &m_vScaleMatrix);
	m_vScale = vVecScale;
	D3DXMatrixScaling(&m_vScaleMatrix, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixMultiply(&m_vViewMatrix, &m_vViewMatrix, &m_vScaleMatrix);
}

void CD3DCamera::Normalize()
{
	D3DXVec3Normalize(&m_vUpDir, &m_vUpDir);
	D3DXVec3Normalize(&m_vViewDir, &m_vViewDir);
	D3DXVec3Normalize(&m_vRightDir, &m_vRightDir);
}

void CD3DCamera::SetViewMatrix()
{
	D3DXMatrixLookAtLH(&m_vViewMatrix, &m_vEyePt, &m_vLookAtPt, &m_vUpDir);
	D3DXMatrixMultiply(&m_vViewMatrix, &m_vViewMatrix, &m_vScaleMatrix);
}

void CD3DCamera::SetViewParams(D3DXVECTOR3& vEyePt, D3DXVECTOR3& vLookAtPt, D3DXVECTOR3& vVecUp)
{
	m_vEyePt = vEyePt; m_vLookAtPt = vLookAtPt;
	m_vUpDir = vVecUp;
	m_vViewDir = m_vLookAtPt - m_vEyePt;
	D3DXVec3Cross(&m_vRightDir, &m_vUpDir, &m_vViewDir);
	D3DXVec3Cross(&m_vUpDir, &m_vViewDir, &m_vRightDir);
	Normalize();
	SetViewMatrix();
}

void CD3DCamera::SetViewParams(D3DXVECTOR3& vEyePt, D3DXVECTOR3& vVecUp, D3DXVECTOR3& vLookDir, float distance)
{
	m_vEyePt = vEyePt;
	D3DXVECTOR3 vViewDir;
	D3DXVec3Normalize(&vViewDir, &vLookDir);
	m_vLookAtPt = m_vEyePt + distance*vViewDir;
	m_vViewDir = vViewDir;
	D3DXVec3Cross(&m_vRightDir, &vVecUp, &m_vViewDir);
	D3DXVec3Cross(&m_vUpDir, &m_vViewDir, &m_vRightDir);
	Normalize();
	SetViewMatrix();
}

void CD3DCamera::SetProjParams(float fovy, float aspect, float zn /*= 1.0f*/, float zf /*= 3000.0f*/)
{
	m_fFov = fovy;
	m_fAspect = aspect;
	m_fNearPlane = zn;
	m_fFarPlane = zf;
	D3DXMatrixPerspectiveFovLH(&m_vProjMatrix, m_fFov, m_fAspect, m_fNearPlane, m_fFarPlane);
	
}

void CD3DCamera::SetProjParams(float width, float height, float plane /*= 1.0f*/, float zn /*= 1.0f*/, float zf /*= 3000.0f*/)
{
	m_fAspect = width / height;
	m_fFov = 2 * atan(height / plane);
	m_fNearPlane = zn;
	m_fFarPlane = zf;
	D3DXMatrixPerspectiveFovLH(&m_vProjMatrix, m_fFov, m_fAspect, m_fNearPlane, m_fFarPlane);
}

float CD3DCamera::GetFov()
{
	return m_fFov;
}

float CD3DCamera::GetAspect()
{
	return m_fAspect;
}

float CD3DCamera::GetNearPlane()
{
	return m_fNearPlane;
}

float CD3DCamera::GetFarPlane()
{
	return m_fFarPlane;
}

D3DXVECTOR3 CD3DCamera::GetEyePoint()
{
	return m_vEyePt;
}

D3DXVECTOR3 CD3DCamera::GetLookAtPoint()
{
	return m_vLookAtPt;
}

D3DXVECTOR3 CD3DCamera::GetViewDir()
{
	return m_vViewDir;
}

D3DXVECTOR3 CD3DCamera::GetRightDir()
{
	return m_vRightDir;
}

D3DXVECTOR3 CD3DCamera::GetUpDir()
{
	return m_vUpDir;
}

D3DXMATRIX CD3DCamera::GetViewMatrix()
{
	return m_vViewMatrix;
}

D3DXMATRIX CD3DCamera::GetProjMatrix()
{
	return m_vProjMatrix;
}


