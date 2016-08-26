#include "FPS.h"
#include<cstdio>

D3DFPS::D3DFPS(IDirect3DDevice9* pDevice) :m_dwCurTime(0), m_dwElapseTime(0), m_dwPreTime(0), m_dwDeltaTime(0), m_fFPS(0.0f), m_dwCount(0), m_bStart(false)
{
	m_pDevice = pDevice;
	m_pFont = new D3DFont("Times New Roman", 24, D3DFONT_ZENABLE);
	if (m_pFont)
	{
		LRESULT hr = 0;
		hr = m_pFont->Init(m_pDevice);
		if (FAILED(hr))
			Delete<D3DFont*>(m_pFont);
	}
}

D3DFPS::~D3DFPS()
{
	Delete<D3DFont*>(m_pFont);
}

void D3DFPS::Start()
{
	m_bStart = true;
	m_dwPreTime = timeGetTime();
}

void D3DFPS::Tick(DWORD& dwDeltaTimes)
{
	if (m_bStart)
	{
		m_dwCount++;
		m_dwCurTime = timeGetTime();
		m_dwDeltaTime = m_dwCurTime - m_dwPreTime;
		m_dwElapseTime += m_dwDeltaTime;
		m_dwPreTime = m_dwCurTime;
		if (m_dwElapseTime >= 1000)
		{
			m_fFPS = (float)m_dwCount*1000.0f/(float)m_dwElapseTime;
			m_dwElapseTime = 0;
			m_dwCount = 0;
		}
		dwDeltaTimes = m_dwDeltaTime;
	}
}

float D3DFPS::GetFPS()
{
	return m_fFPS;
}

void D3DFPS::Stop()
{
	m_bStart = false;
	m_fFPS = 0.0f;
}

DWORD D3DFPS::GetDeltaTime()
{
	return m_dwDeltaTime;
}

bool D3DFPS::Render(D3DCOLOR color)
{
	if (m_pDevice)
	{
		sprintf(m_cFpsString, "%f", m_fFPS);

		if (m_pFont)
		{
			HRESULT hr = 0;
			//hr = m_pFont->DrawText(0, 0, m_cFpsString, color, D3DTEXT_HORIZONTAL, D3DTEXT_CENTER_X | D3DTEXT_TOP, D3DTEXT_HCENTER);
			//hr = m_pFont->DrawText(0, 0, TEXT("ajdkfljalsk\n\n\rksjfk\raksjkfla\n\n\n"), color, D3DTEXT_VERTICAL, D3DTEXT_CENTER_X | D3DTEXT_CENTER_Y, D3DTEXT_VTOP);
			hr = m_pFont->DrawText3D(TEXT("ajdkfljalsk\nksjfk\raksjkfla\n\n\n"), D3DXVECTOR3(0.0f, 5.0f, 0.0f), 0xffff0000);
			if (FAILED(hr))
				return false;
		}
		else
			return false;
	}
	return true;
}