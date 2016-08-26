#ifndef _ARCBALL_H__
#define _ARCBALL_H__
#include <D3D9.h>
#include<D3DX9Math.h>

class ArcBall
{
	bool m_bDraged;
	int m_vWidth;
	int m_vHeight;

	int m_vCenterX;
	int m_vCenterY;

	D3DXQUATERNION m_vStartQuat;
	D3DXQUATERNION m_vPreQuat;
	D3DXQUATERNION m_vCurQuat;
	D3DXQUATERNION m_vIncreQuat;
	
	D3DXVECTOR3 m_vStartPoint;
	D3DXVECTOR3 m_vPrePoint;
	D3DXVECTOR3 m_vCurPoint;

public:
	LRESULT DealWindwosMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void GetAxisAngle(D3DXVECTOR3* pAxis,float* pAngle);
public:
	ArcBall(unsigned int vWidth, unsigned int vHeight);
	~ArcBall();
private:
	void OnRightBtnDown(int x,int y);
	void OnMouseMove(int x, int y);
	void OnRightBtnUp(int x,int y);

	D3DXVECTOR3 FromScreenTo3DBall(int x, int y);
	D3DXQUATERNION ComputeQuatFromBallPoints(D3DXVECTOR3& vPrePt, D3DXVECTOR3& vCurPt);
	
};


#endif