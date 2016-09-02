#include "D3DSprite.h"
#include "D3DCommon.h"

D3DSprite::D3DSprite(IDirect3DDevice9* pDevice) :m_pSprite(NULL)
{
	if (pDevice)
	{
		D3DXCreateSprite(pDevice, &m_pSprite);
	}
	D3DXMatrixIdentity(&m_OrgMatrix);
	m_Center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

D3DSprite::~D3DSprite()
{
	Release();
}

void D3DSprite::SetCenter(D3DXVECTOR3& center)
{
	m_Center = center;
}

void D3DSprite::Release()
{
	::Release<ID3DXSprite*>(m_pSprite);
}

bool D3DSprite::Begin(DWORD Flags)
{
	LRESULT hr = 0;
	hr = m_pSprite->Begin(Flags);
	if (FAILED(hr))
		return false;
	return true;
}

bool D3DSprite::End()
{
	LRESULT hr = 0;
	hr = m_pSprite->End();
	if (FAILED(hr))
		return false;
	return true;
}

bool D3DSprite::Draw(IDirect3DTexture9* pTexture, POINT& pos, RECT& surrect, D3DCOLOR color)
{
	LRESULT hr = 0;
	hr = m_pSprite->Draw(pTexture,&surrect,&m_Center,&D3DXVECTOR3(pos.x,pos.y,0.0f),color);
	if (FAILED(hr))
		return false;
	return true;
}

bool D3DSprite::Draw(CTexture2D* pTexture, POINT& pos, D3DCOLOR color)
{
	if (!pTexture)
		return false;
	RECT rect = pTexture->GetRect();
	return Draw(pTexture, pos, rect, color);
}

bool D3DSprite::Draw(CTexture2D* pTexture, POINT& pos, RECT& surrect, D3DCOLOR color)
{
	if (!pTexture)
		return false;
	IDirect3DTexture9* pTex = pTexture->GetTexture();
	return Draw(pTex, pos, surrect, color);
}

bool D3DSprite::Draw(CTexture2D* pTexture, D3DXMATRIX* pTransform, POINT& pos, D3DCOLOR color)
{
	if (!pTexture)
		return false;
	LRESULT hr = 0;
	hr = m_pSprite->SetTransform(pTransform);
	if (FAILED(hr))
		return false;
	bool result = false;
	result = Draw(pTexture, pos, color);
	hr = m_pSprite->SetTransform(&m_OrgMatrix);
	if (FAILED(hr))
		return false;
	return result;
}

bool D3DSprite::Draw(CTexture2D* pTexture, D3DXMATRIX* pTransform, POINT& pos, RECT& surrect, D3DCOLOR color)
{
	if (!pTexture)
		return false;
	LRESULT hr = 0;
	hr = m_pSprite->SetTransform(pTransform);
	if (FAILED(hr))
		return false;
	bool result = false;
	result = Draw(pTexture, pos, surrect, color);
	hr = m_pSprite->SetTransform(&m_OrgMatrix);
	if (FAILED(hr))
		return false;
	return result;
}

bool D3DSprite::Draw(CTexture2D* pTexture, POINT& pos, SIZE& size, RECT& surrect, D3DCOLOR color)
{
	if (!pTexture)
		return false;
	float ScaleY = size.cy / (float)(surrect.bottom - surrect.top);
	float ScaleX = size.cx / (float)(surrect.right - surrect.left);
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, ScaleX, ScaleY, 0.0f);
	return Draw(pTexture, &scale, pos, surrect, color);
}

bool D3DSprite::Draw(CTexture2D* pTexture, RECT& destrect, RECT& surrect, D3DCOLOR color)
{
	if (!pTexture)
		return false;
	POINT pos;
	pos.x = destrect.left;
	pos.y = destrect.top;
	SIZE size;
	size.cx = destrect.right - destrect.left;
	size.cy = destrect.bottom - destrect.top;
	return Draw(pTexture, pos, size, surrect, color);
}

bool D3DSprite::Draw(CTexture2D* pTexture, RECT& destrect, D3DCOLOR color)
{
	if (!pTexture)
		return false;
	RECT surrect = pTexture->GetRect();
	return Draw(pTexture, destrect, surrect, color);
}



