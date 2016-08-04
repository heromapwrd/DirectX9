#ifndef _FPS_H__
#define _FPS_H__
#include "d3dfont.h"

class FPSCounter
{
public:
	FPSCounter(IDirect3DDevice9* pDevice);
	~FPSCounter();

	bool Render(D3DCOLOR color, float timeDelta);

private:
	IDirect3DDevice9* m_pDevice;
	CD3DFont* m_pFont;
	DWORD m_dwFrameCount;
	float m_fTimeElapse;
	float m_fFps;
	char m_cFpsString[9];
};

#endif