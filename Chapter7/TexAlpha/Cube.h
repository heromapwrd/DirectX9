/*
-----------------------------------------------------
Cube.h 完成程序创建,消息循环工作
-----------------------------------------------------
*/

#ifndef _CUBE_H__
#define _CUBE_H__
#include "Vertex.h"

class Cube
{
public:
	Cube(IDirect3DDevice9* pDevice);
	~Cube();
	bool Draw(D3DXMATRIX* pWorld, D3DMATERIAL9* pMtrl, IDirect3DTexture9* pTX);
private:
	void Init();
private:
	IDirect3DDevice9*		m_pDevice;
	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DIndexBuffer9*  m_pIB;
};

#endif
