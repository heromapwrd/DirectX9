#ifndef _D3DSPRITE_H__
#define _D3DSPRITE_H__
#include<d3d9.h>
#include<d3dx9.h>
#include "CTexture2D.h"

class D3DSprite
{
	ID3DXSprite* m_pSprite;
	D3DXMATRIX m_OrgMatrix;
	D3DXVECTOR3 m_Center;

public:
	bool Begin(DWORD Flags);
	bool End();
	
	bool Draw(CTexture2D* pTexture, POINT& pos, D3DCOLOR color);
	bool Draw(CTexture2D* pTexture, POINT& pos, RECT& surrect, D3DCOLOR color);
	bool Draw(CTexture2D* pTexture, D3DXMATRIX* pTransform, POINT& pos, D3DCOLOR color);
	bool Draw(CTexture2D* pTexture, D3DXMATRIX* pTransform, POINT& pos, RECT& surrect, D3DCOLOR color);
	bool Draw(CTexture2D* pTexture, POINT& pos, SIZE& size, RECT& surrect, D3DCOLOR color);
	bool Draw(CTexture2D* pTexture, RECT& destrect, RECT& surrect, D3DCOLOR color);
	bool Draw(CTexture2D* pTexture, RECT& destrect, D3DCOLOR color);

public:
	void SetCenter(D3DXVECTOR3& center);
	void Release();
public:
	D3DSprite(IDirect3DDevice9* pDevice);
	~D3DSprite();

private:
	bool Draw(IDirect3DTexture9* pTexture, POINT& pos, RECT& surrect, D3DCOLOR color);
};


#endif  //_D3DSPRITE_H__