#include "fps.h"
#include<cstdio>

FPSCounter::FPSCounter(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	m_pFont = new CD3DFont("Times New Roman", 24, 0);
	m_pFont->InitDeviceObjects(m_pDevice);
	m_pFont->RestoreDeviceObjects();
	m_dwFrameCount = 0;
	m_fTimeElapse = 0.0f;
	m_fFps = 0.0f;
}

FPSCounter::~FPSCounter()
{
	if (m_pFont)
	{
		m_pFont->InvalidateDeviceObjects();
		m_pFont->DeleteDeviceObjects();
		delete m_pFont;
	}
}

bool FPSCounter::Render(D3DCOLOR color, float timeDelta)
{
	if (m_pFont)
	{
		m_dwFrameCount++;
		m_fTimeElapse += timeDelta;
		if (m_fTimeElapse>=1.0f)
		{
			m_fFps = m_dwFrameCount / m_fTimeElapse;

			sprintf(m_cFpsString, "%f", m_fFps);
			m_dwFrameCount = 0;
			m_fTimeElapse = 0.0f;
		}


		m_pFont->DrawText(20, 20, color, "This is a test,\nOK\nOk\nOK", D3DFONT_CENTERED_X | D3DFONT_CENTERED_Y);
		//m_pFont->Render3DText(m_cFpsString, D3DFONT_CENTERED_X);
		//m_pFont->DrawText(0, 0, color, m_cFpsString, D3DFONT_CENTERED_X|D3DFONT_CENTERED_Y);
		//m_pFont->DrawTextScaled(0.0f, 0.0f, 0.001f, 0.1f, 0.1f, color, m_cFpsString,D3DFONT_CENTERED_X|D3DFONT_CENTERED_Y);
		//m_pFont->DrawTextScaled(0.0f, 0.0f, 0.001f, 0.1f, 0.1f, color, "This is a test,\nOK\nOk\nOK", D3DFONT_CENTERED_X | D3DFONT_CENTERED_Y);

	}
	return true;
}