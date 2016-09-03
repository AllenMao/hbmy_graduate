#pragma once
#include "Timer.h"

CTimer::CTimer(void)
{
	InitTimer();
}

CTimer::~CTimer(void)
{
}
void CTimer::InitTimer()
{
	m_iFrameCount = 0;
	m_fFPS = 0;
	m_fTime1 = 0;
	m_fTime2 = 0;
	m_fTimeSlice = 0;
	if(QueryPerformanceFrequency((LARGE_INTEGER*) &m_int64OneSecondTicks))//返回硬件支持的高精度计数器的频率,不支持则为0
	{
		m_bUseTime = true;
		QueryPerformanceCounter((LARGE_INTEGER*) &m_int64TimeStartCounts);//用于得到高精度计时器的值
	}
	else
	{
		m_bUseTime = false;
		m_ulTimeStart = timeGetTime();
	}
}
float CTimer::GetPlayTime()
{
	if(m_bUseTime)
	{
		__int64 int64TimeCurrentCounts;
		QueryPerformanceCounter((LARGE_INTEGER*) &int64TimeCurrentCounts);
		return ((float) ((int64TimeCurrentCounts - m_int64TimeStartCounts) * (1.0f / m_int64OneSecondTicks) * 1000.0f));
	}
	else
	{
		return ((float) (timeGetTime() - m_ulTimeStart));
	}
}

void CTimer::UpdateFPS()
{
	m_iFrameCount++;
	if(m_iFrameCount%5==1)
	{
		m_fTime1 = GetPlayTime() / 1000;
	}
	else if(m_iFrameCount%5==0)
	{
		m_fTime2 = GetPlayTime() / 1000;
		m_fTimeSlice = (float)fabs(m_fTime1 - m_fTime2);
	}
	m_fFPS = 5 / m_fTimeSlice;
}