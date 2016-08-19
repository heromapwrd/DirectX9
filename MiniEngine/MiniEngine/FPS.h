#ifndef _FPS_H__
#define _FPS_H__
#include<D3D9.h>
#include<time.h>

class FPS
{
	DWORD m_dwPreTime;
	DWORD m_dwCurTime;
	DWORD m_dwElapseTime;
	float m_fFPS;
	DWORD m_dwCount;
	bool m_bStart;
public:
	void Start();
	void Tick(DWORD times);
	float GetFPS();
	void Stop();
public:
	FPS();
	~FPS();
};

#endif //_FPS_H__