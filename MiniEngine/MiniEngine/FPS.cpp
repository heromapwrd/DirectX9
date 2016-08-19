#include "FPS.h"

FPS::FPS() :m_dwCurTime(0), m_dwElapseTime(0), m_dwPreTime(0), m_fFPS(0.0f), m_dwCount(0), m_bStart(false)
{

}

FPS::~FPS()
{

}

void FPS::Start()
{
	m_bStart = true;
	m_dwPreTime = timeGetTime();
}

void FPS::Tick(DWORD times)
{
	if (m_bStart)
	{
		m_dwCount++;
		m_dwCurTime = timeGetTime();
		m_dwElapseTime += m_dwPreTime - m_dwPreTime;
		m_dwPreTime = m_dwCurTime;
		if (m_dwElapseTime >= 1000)
		{
			m_fFPS = (float)m_dwElapseTime / (float)m_dwCount;
			m_dwElapseTime = 0;
			m_dwCount = 0;
		}
	}
}

float FPS::GetFPS()
{
	return m_fFPS;
}

void FPS::Stop()
{
	m_bStart = false;
	m_fFPS = 0.0f;
}

	