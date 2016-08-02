#ifndef _CAMERA_H__
#define _CAMERA_H__
#include<d3dx9.h>

class Camera

{
public:
	enum CameraType{LANDOBJECT,AIRCRAFT};
	Camera();
	Camera(CameraType type);
	~Camera();

	void Pitch(float angle);
	void Yaw(float angle);
	void Roll(float angle);

	void Walk(float unites);
	void Strafe(float unites);
	void Fly(float unites);

	void GetViewMatrix(D3DXMATRIX * pMatrix);
	void SetCamereType(CameraType type);
	void GetPos(D3DXVECTOR3* pPos);
	void SetPos(D3DXVECTOR3* pPos);

	void GetLook(D3DXVECTOR3* pLook);
	void GetRight(D3DXVECTOR3* pRight);
	void GetUp(D3DXVECTOR3* pUp);

private:
	CameraType m_Type;
	D3DXVECTOR3 m_Look;
	D3DXVECTOR3 m_Up;
	D3DXVECTOR3 m_Right;
	D3DXVECTOR3 m_Pos;
};

#endif