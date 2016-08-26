#ifndef _FPS_H__
#define _FPS_H__
#include<time.h>
#include "D3DFont.h"

class D3DFPS
{
	DWORD m_dwPreTime;
	DWORD m_dwCurTime;
	DWORD m_dwElapseTime;
	DWORD m_dwDeltaTime;
	float m_fFPS;
	DWORD m_dwCount;
	bool m_bStart;
	IDirect3DDevice9* m_pDevice;
	D3DFont* m_pFont;
	TCHAR m_cFpsString[9];
public:
	void Start();
	void Tick(DWORD& dwDeltaTimes);
	float GetFPS();
	void Stop();
	DWORD GetDeltaTime();
	bool Render(D3DCOLOR color);
public:
	D3DFPS(IDirect3DDevice9* pDevice);
	~D3DFPS();
};

#endif //_FPS_H__