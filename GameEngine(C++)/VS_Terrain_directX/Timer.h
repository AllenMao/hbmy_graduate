#pragma once
#include "Common.h"
class CTimer
{
public:
	CTimer(void);
	~CTimer(void);
	void InitTimer();
	float GetPlayTime();
	void UpdateFPS();
	inline float GetFPS()
	{
		return m_fFPS;
	}
private:
	bool m_bUseTime;
	__int64 m_int64OneSecondTicks;
	__int64 m_int64TimeStartCounts;
	unsigned long m_ulTimeStart;
	int m_iFrameCount;
	float m_fFPS;
	float m_fTime1,m_fTime2,m_fTimeSlice;
};