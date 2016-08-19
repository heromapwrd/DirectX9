#ifndef _COUNTER_H__
#define _COUNTER_H__
#include<D3D9.h>
#include<time.h>

class Counter
{
	DWORD m_dwPauseTime;
	DWORD m_dwRestoreTime;
	DWORD m_dwTotalElapseTime;
	DWORD m_dwElapseTime;
	DWORD m_dwPeriodTime;
	DWORD m_dwCurTime;

	bool m_bStarted;
	bool m_bPaused;
public:
	void Tick(DWORD times);
	void SetPeriod(DWORD period);
	void ResetToPeriod(bool bPeriod);
	void Increment(DWORD times);
	void Decrement(DWORD times);
	DWORD GetCurTime();
	bool IsStoped();
	bool IsPaused();
	bool IsFull();
	void Start();
	void Pause();
	void Restore();
	void Stop();
	void Reset();
public:
	Counter();
	~Counter();
};

#endif //_COUNTER_H__