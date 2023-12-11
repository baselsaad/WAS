#include "Timer.h"
#include "MyAssertTool.h"


Timer::Timer()
	: m_IsTimerStarted(false)
	, m_Name("Unknown")
{
}

Timer::Timer(const char* name)
	: m_Name(name)
{
	StartTimer();
}

Timer::~Timer()
{
	StopTimer();

	Debug::LogProfile("(%s): %f ms", *m_Name, ElapsedTimeInMili());
}

void Timer::StartTimer() const
{
	m_Start = std::chrono::high_resolution_clock::now();
	m_IsTimerStarted = true;
}

void Timer::StopTimer() const
{
	m_End = std::chrono::high_resolution_clock::now();
	ASSERT_BREAK(m_IsTimerStarted, "Timer should start first!");

	m_Duration = m_End - m_Start;
	m_ElapsedTime = m_Duration.count() * 1000.0f;
	m_IsTimerStarted = false;
}

