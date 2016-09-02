#ifndef _CTEXTURE2D_H__
#define _CTEXTURE2D_H__
#include<d3d9.h>
#include<d3dx9.h>
#include<tchar.h>

class CTexture2D
{
	IDirect3DDevice9* m_pDevice;
	IDirect3DTexture9* m_pTexture;		
	D3DXIMAGE_INFO m_ImageInfo;
public:
	bool LoadImage(const TCHAR* filename);
	void Release();
public:
	RECT GetRect();
	D3DXIMAGE_INFO GetInfo();
	IDirect3DTexture9* GetTexture();
public:
	CTexture2D(IDirect3DDevice9* pDevice);
	~CTexture2D();
};


#endif //_CTEXTRUE2D_H__