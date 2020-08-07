#pragma once
#include<Windows.h>

class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency;
	float m_tickPerMs;
	INT64 m_startTime;
	float m_frameTime;
};

