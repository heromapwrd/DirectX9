#ifndef _D3DCAMERA_H__
#define _D3DCAMERA_H__
#include<D3D9.h>
#include<D3DX9Math.h>

D3DXVECTOR3 g_UpDir(0.0f, 1.0f, 0.0f);

enum CameraType
{
	CT_FLYFREEDOM,
	CT_FIRSTPERSON,
	CT_THIRDPERSON,
	CT_NUM
};

class CD3DCamera
{
	CameraType	m_eCameraType;

	D3DXVECTOR3 m_vEyePt;
	D3DXVECTOR3 m_vLookAtPt;
	D3DXVECTOR3 m_vViewDir;
	D3DXVECTOR3 m_vUpDir;
	D3DXVECTOR3 m_vRightDir;
	D3DXVECTOR3 m_vScale;
	
	float m_fFov;
	float m_fAspect;
	float m_fNearPlane;
	float m_fFarPlane;
	float m_fWalkSpeed;

	D3DXMATRIX m_vViewMatrix;
	D3DXMATRIX m_vProjMatrix;
	D3DXMATRIX m_vScaleMatrix;
	
public:
	void Rotate(float angle, D3DXVECTOR3* pAxis, D3DXVECTOR3* pPoint = NULL);
	void Rotate(D3DXVECTOR3 axis, float angle);
	void Pitch(float angle);
	void Yaw(float angle);
	void Roll(float angle);

	void WalkView(float times);
	void WalkRight(float times);
	void FlayUP(float times);

	void SetWalkSpeed(float fSpeed);
	void SetScale(D3DXVECTOR3& vVecScale);
	void SetViewParams(D3DXVECTOR3& vEyePt, D3DXVECTOR3& vLookAtPt, D3DXVECTOR3& vVecUp);
	void SetViewParams(D3DXVECTOR3& vEyePt, D3DXVECTOR3& vVecUp, D3DXVECTOR3& vLookDir,float distance);
	void SetViewMatrix();
	void Normalize();
	void SetProjParams(float fovy, float aspect, float zn = 1.0f, float zf = 3000.0f);
	void SetProjParams(float width, float height, float plane = 1.0f, float zn = 1.0f, float zf = 3000.0f);
	

	float		GetFov();
	float		GetAspect();
	float		GetNearPlane();
	float		GetFarPlane();
	D3DXVECTOR3 GetEyePoint();
	D3DXVECTOR3 GetLookAtPoint();
	D3DXVECTOR3 GetViewDir();
	D3DXVECTOR3 GetRightDir();
	D3DXVECTOR3 GetUpDir();
	D3DXMATRIX	GetViewMatrix();
	D3DXMATRIX	GetProjMatrix();

public:
	CD3DCamera();
	~CD3DCamera();

};

#endif //_D3DCAMERA_H__