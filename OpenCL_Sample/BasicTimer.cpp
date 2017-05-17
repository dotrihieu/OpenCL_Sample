#include "BasicTimer.h"

#ifdef OS_ANDROID
#include <time.h>
#endif
#ifdef _WIN32
#include <time.h>
#include <windows.h>
#endif

BasicTimer::BasicTimer()
{
#ifdef _WIN32
	if (!QueryPerformanceFrequency(&m_frequency))
	{
		//throw ref new Platform::FailureException();
	}
#endif
	Reset();
}

// Reset the timer to initial values.
void BasicTimer::Reset()
{
	Update();
	m_startTime = m_currentTime;
	m_total = 0.0f;
	m_delta = 1.0f / 60.0f;
}

// Update the timer's internal values.
void BasicTimer::Update()
{
#ifdef _WIN32
	if (!QueryPerformanceCounter(&m_currentTime))
	{
		//throw ref new Platform::FailureException();
	}

	m_total = static_cast<float>(
		static_cast<double>(m_currentTime.QuadPart - m_startTime.QuadPart) /
		static_cast<double>(m_frequency.QuadPart)
		);
	
	if (m_lastTime.QuadPart == m_startTime.QuadPart)
	{
		// If the timer was just reset, report a time delta equivalent to 60Hz frame time.
		m_delta = 1.0f / 60.0f;
	}
	else
	{
		m_delta = static_cast<float>(
			static_cast<double>(m_currentTime.QuadPart - m_lastTime.QuadPart) /
			static_cast<double>(m_frequency.QuadPart)
			);
	}
#elif defined(OS_ANDROID)
	m_currentTime = GetCurrentTimeInNanoSecond();
	m_delta = float(m_currentTime - m_lastTime) / float(1000000000LL);
	m_total = float(m_currentTime - m_startTime) / float(1000000000LL);
#endif
	m_lastTime = m_currentTime;
}

// Duration in seconds between the last call to Reset() and the last call to Update().
float BasicTimer::GetTotalAtFrameBegin()
{
	return m_total; 
}

// Duration in seconds between the previous two calls to Update().
float BasicTimer::GetDeltaAtFrameBegin()
{
	return m_delta;
}

float BasicTimer::GetCurrentTimeInSecond()
{
#ifdef _WIN32
	if (!QueryPerformanceCounter(&m_currentTime))
	{
		//throw ref new Platform::FailureException();
	}

	return static_cast<float>(
		static_cast<double>(m_currentTime.QuadPart - m_startTime.QuadPart) /
		static_cast<double>(m_frequency.QuadPart)
		);
#elif defined(OS_ANDROID)
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (float)now.tv_sec  + float(now.tv_nsec) / float(1000000000LL);
#endif
}

int64_t BasicTimer::GetCurrentTimeInNanoSecond()
{
#ifdef _WIN32
	struct timespec now;
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	LARGE_INTEGER value;
	QueryPerformanceCounter(&value);
	__int64 nano = (1000000000LL * value.QuadPart) / s_frequency.QuadPart;
	return nano;
#elif defined(OS_ANDROID)
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (int64_t)now.tv_sec * 1000000000LL + now.tv_nsec;
#endif
}