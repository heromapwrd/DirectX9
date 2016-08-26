#include "Counter.h"

Counter::Counter()
{
	Reset();
}

Counter::~Counter()
{

}

void Counter::SetPeriod(DWORD period)
{
	m_dwPeriodTime = period;
}

void Counter::ResetToPeriod(bool bPeriod)
{
	DWORD periodtime = m_dwPeriodTime;
	Reset();
	SetPeriod(periodtime);
	m_dwCurTime = bPeriod ? m_dwPeriodTime : 0;
}

void Counter::Increment(DWORD times)
{
	if (m_bStarted)
	{
		m_dwTotalElapseTime += times;
		if (!m_bPaused)
		{
			m_dwCurTime += times;
			Tick(times);
		}
		
	}
}

void Counter::Decrement(DWORD times)
{
	if (m_bStarted)
	{
		m_dwTotalElapseTime += times;
		if (!m_bPaused)
		{
			m_dwCurTime -= times;
			Tick(times);
		}
	}
}

void Counter::Tick(DWORD times)
{
	m_dwElapseTime += times;
	if (IsFull())
		Stop();
}


bool Counter::IsStoped()
{
	return !m_bStarted;
}

bool Counter::IsPaused()
{
	return m_bPaused;
}

bool Counter::IsFull()
{
	if (m_dwElapseTime >= m_dwPeriodTime)
		return true;
	return false;
}

DWORD Counter::GetCurTime()
{
	if (m_bStarted)
	{
		return m_dwCurTime;
	}
	else
		return m_dwPeriodTime;
}


void Counter::Start()
{
	ResetToPeriod(false); 
	m_bStarted = true;
}

void Counter::Pause()
{
	m_dwPauseTime = timeGetTime();
	m_bPaused = true;
}

void Counter::Restore()
{
	m_dwRestoreTime = timeGetTime();
	m_bPaused = false;
}

void Counter::Stop()
{
	ResetToPeriod(false);
}

void Counter::Reset()
{
	m_bPaused = false;
	m_bStarted = false;

	m_dwCurTime = 0;
	m_dwPauseTime = 0;
	m_dwElapseTime = 0;
	m_dwPeriodTime = 0;
	m_dwRestoreTime = 0;
	m_dwTotalElapseTime = 0;
}
