#include "ArcBall.h"

ArcBall::ArcBall(unsigned int vWidth, unsigned int vHeight) :m_vWidth(vWidth), m_vHeight(vHeight), m_bDraged(false)
{
	m_vCenterX = m_vWidth / 2;
	m_vCenterY = m_vHeight / 2;
	m_vPreQuat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
	m_vCurQuat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
	m_vStartQuat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
	m_vIncreQuat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
}

ArcBall::~ArcBall()
{

}

LRESULT ArcBall::DealWindwosMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	
	switch (uMsg)
	{
	case WM_RBUTTONDOWN:
		OnRightBtnDown(x, y);
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(x, y);
		break;

	case WM_RBUTTONUP:
		OnRightBtnUp(x, y);
		break;

	default:
		break;
	}
	return 0;
}

void ArcBall::GetAxisAngle(D3DXVECTOR3* pAxis, float* pAngle)
{
	D3DXQuaternionToAxisAngle(&m_vIncreQuat, pAxis, pAngle);
}

void ArcBall::OnRightBtnDown(int x, int y)
{
	if (x >= 0 && x < m_vWidth&&y >= 0 && y < m_vHeight)
	{
		m_vStartPoint = FromScreenTo3DBall(x, y);
		m_vPrePoint = m_vStartPoint;
		m_vPreQuat = m_vStartQuat;
		m_bDraged = true;
	}
	
}

void ArcBall::OnMouseMove(int x, int y)
{
	if (m_bDraged)
	{
		if (x >= 0 && x < m_vWidth&&y >= 0 && y < m_vHeight)
		{
			m_vCurPoint = FromScreenTo3DBall(x, y);
			m_vIncreQuat = ComputeQuatFromBallPoints(m_vPrePoint, m_vCurPoint);
			m_vPrePoint = m_vCurPoint;
			m_vCurQuat = m_vPreQuat*m_vIncreQuat;
			m_vPreQuat = m_vCurQuat;
		}
	}
}

void ArcBall::OnRightBtnUp(int x, int y)
{
	if (m_bDraged)
	{
		if (x >= 0 && x < m_vWidth&&y >= 0 && y < m_vHeight)
		{
			m_vStartQuat = m_vCurQuat;
			m_bDraged = false;
		}
	}
}

D3DXVECTOR3 ArcBall::FromScreenTo3DBall(int x, int y)
{
	float fx = -(x - m_vCenterX) / (m_vWidth / 2);
	float fy = (y - m_vCenterY) / (m_vHeight / 2);
	float fz = 0.0f;
	float mag = sqrtf(fx*fx + fy*fy);
	if (mag > 1.0f)
	{
		fx /= mag;
		fy /= mag;
	}
	else
	{
		fz = sqrtf(1.0 - mag*mag);
	}
	return D3DXVECTOR3(fx, fy, fz);
}

D3DXQUATERNION ArcBall::ComputeQuatFromBallPoints(D3DXVECTOR3& vPrePt, D3DXVECTOR3& vCurPt)
{
	D3DXVECTOR3 prept, curpt;
	D3DXVec3Normalize(&prept, &vPrePt);
	D3DXVec3Normalize(&curpt, &vCurPt);

	float angle = acosf(D3DXVec3Dot(&prept, &curpt));

	D3DXVECTOR3 n;
	D3DXVec3Cross(&n,&prept, &curpt);
	D3DXVec3Normalize(&n, &n);
	return D3DXQUATERNION(n.x*sinf(angle / 2.0f), n.y*sinf(angle / 2.0f), n.z*sinf(angle / 2.0f), cosf(angle / 2.0f));
}