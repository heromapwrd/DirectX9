#include "CTexture2D.h"
#include "D3DCommon.h"

CTexture2D::CTexture2D(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	m_pTexture = NULL;
	memset((void*)&m_ImageInfo, 0, sizeof(D3DXIMAGE_INFO));
}

CTexture2D::~CTexture2D()
{
	Release();
}

bool CTexture2D::LoadImage(const TCHAR* filename)
{
	if (!m_pDevice)
		return false;
	LRESULT hr = 0;
	hr = D3DXCreateTextureFromFileEx(
		m_pDevice, 
		filename, 
		D3DX_DEFAULT_NONPOW2, 
		D3DX_DEFAULT_NONPOW2, 
		0, 
		0, 
		D3DFMT_UNKNOWN, 
		D3DPOOL_MANAGED, 
		D3DX_DEFAULT, 
		D3DX_DEFAULT, 
		0xFF000000, 
		&m_ImageInfo, 
		NULL, 
		&m_pTexture
		);
	if (FAILED(hr))
		return false;
	return true;
}

void CTexture2D::Release()
{
	::Release<IDirect3DTexture9*>(m_pTexture);
}

RECT CTexture2D::GetRect()
{
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = m_ImageInfo.Width;
	rect.bottom = m_ImageInfo.Height;
	return rect;
}

IDirect3DTexture9* CTexture2D::GetTexture()
{
	return m_pTexture;
}

D3DXIMAGE_INFO CTexture2D::GetInfo()
{
	return m_ImageInfo;
}

