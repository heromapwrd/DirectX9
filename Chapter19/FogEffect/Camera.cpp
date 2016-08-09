#include "Camera.h"

Camera::Camera():
m_Type(LANDOBJECT), m_Pos(D3DXVECTOR3(0.0f, 0.0f, 0.0f)), m_Look(D3DXVECTOR3(0.0f, 0.0f, 1.0f)), m_Right(D3DXVECTOR3(1.0f, 0.0f, 0.0f)), m_Up(D3DXVECTOR3(0.0f,1.0f,0.0f))
{

}

Camera::Camera(CameraType type):
m_Type(type), m_Pos(D3DXVECTOR3(0.0f, 0.0f, 0.0f)), m_Look(D3DXVECTOR3(0.0f, 0.0f, 1.0f)), m_Right(D3DXVECTOR3(1.0f, 0.0f, 0.0f)), m_Up(D3DXVECTOR3(0.0f, 1.0f, 0.0f))
{

}

Camera::~Camera()
{

}

void Camera::Pitch(float angle)
{
	D3DXMATRIX mPitch;
	D3DXMatrixRotationAxis(&mPitch, &m_Right, angle);
	D3DXVec3TransformCoord(&m_Look, &m_Look, &mPitch);
	D3DXVec3TransformCoord(&m_Up, &m_Up, &mPitch);
}

void Camera::Yaw(float angle)
{
	D3DXMATRIX mYaw;
	if (m_Type==LANDOBJECT)
	{
		D3DXMatrixRotationAxis(&mYaw, &D3DXVECTOR3(0.0f,1.0f,0.0f), angle);
	}
	else
	{
		D3DXMatrixRotationAxis(&mYaw, &m_Up, angle);

	}
	D3DXVec3TransformCoord(&m_Look, &m_Look, &mYaw);
	D3DXVec3TransformCoord(&m_Right, &m_Right, &mYaw);
}

void Camera::Roll(float angle)
{
	if (m_Type == AIRCRAFT)
	{
		D3DXMATRIX mRoll;
		D3DXMatrixRotationAxis(&mRoll, &m_Look, angle);
		D3DXVec3TransformCoord(&m_Up, &m_Up, &mRoll);
		D3DXVec3TransformCoord(&m_Right, &m_Right, &mRoll);
	}
}

void Camera::Walk(float unites)
{
	if (m_Type == AIRCRAFT)
	{
		m_Pos += unites*m_Look;
	}
	else
	{
		m_Pos += unites*D3DXVECTOR3(m_Look.x, 0.0f, m_Look.z);
	}
}

void Camera::Strafe(float unites)
{
	if (m_Type == AIRCRAFT)
	{
		m_Pos += unites*m_Right;
	}
	else
	{
		m_Pos += unites*D3DXVECTOR3(m_Right.x, 0.0f, m_Right.z);
	}
}

void Camera::Fly(float unites)
{
	if (m_Type == AIRCRAFT)
	{
		m_Pos += unites*m_Up;
	}
	else
	{
		m_Pos.y += unites;
	}
}

void Camera::GetViewMatrix(D3DXMATRIX * pMatrix)
{
	D3DXVec3Normalize(&m_Look, &m_Look);

	D3DXVec3Cross(&m_Up, &m_Look, &m_Right);
	D3DXVec3Normalize(&m_Up, &m_Up);

	D3DXVec3Cross(&m_Right, &m_Up, &m_Look);
	D3DXVec3Normalize(&m_Right, &m_Right);

	float x = -D3DXVec3Dot(&m_Right, &m_Pos);
	float y = -D3DXVec3Dot(&m_Up, &m_Pos);
	float z = -D3DXVec3Dot(&m_Look, &m_Pos);

	(*pMatrix)(0, 0) = m_Right.x; (*pMatrix)(0, 1) = m_Up.x; (*pMatrix)(0, 2) = m_Look.x; (*pMatrix)(0, 3) = 0.0f;
	(*pMatrix)(1, 0) = m_Right.y; (*pMatrix)(1, 1) = m_Up.y; (*pMatrix)(1, 2) = m_Look.y; (*pMatrix)(1, 3) = 0.0f;
	(*pMatrix)(2, 0) = m_Right.z; (*pMatrix)(2, 1) = m_Up.z; (*pMatrix)(2, 2) = m_Look.z; (*pMatrix)(2, 3) = 0.0f;
	(*pMatrix)(3, 0) = x;        (*pMatrix)(3, 1) = y;     (*pMatrix)(3, 2) = z;       (*pMatrix)(3, 3) = 1.0f;

}

void Camera::SetCamereType(CameraType type)
{
	m_Type = type;
}

void Camera::GetPos(D3DXVECTOR3* pPos)
{
	*pPos = m_Pos;
}

void Camera::SetPos(D3DXVECTOR3* pPos)
{
	m_Pos = *pPos;
}

void Camera::GetLook(D3DXVECTOR3* pLook)
{
	*pLook = m_Look;
}

void Camera::GetRight(D3DXVECTOR3* pRight)
{
	*pRight = m_Right;
}

void Camera::GetUp(D3DXVECTOR3* pUp)
{
	*pUp = m_Up;
}