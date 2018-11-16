#include "Timer.h"



Timer::Timer()
	: _prevPerfCount(LARGE_INTEGER())
	, _totalTime(0.0f)
	, _deltaTime(0.0f)
{
}


Timer::~Timer()
{
}

void Timer::Init()
{
	SR_Assert(QueryPerformanceCounter(&_prevPerfCount));
}

void Timer::Shutdown()
{
	_prevPerfCount = LARGE_INTEGER();
	_deltaTime = 0.0f;
	_totalTime = 0.0f;
}

void Timer::Update()
{
	LARGE_INTEGER perfCount, freq;
	SR_Assert(QueryPerformanceCounter(&perfCount));
	SR_Assert(QueryPerformanceFrequency(&freq));

	_deltaTime = (perfCount.QuadPart - _prevPerfCount.QuadPart) / (double)(freq.QuadPart);
	_totalTime += _deltaTime;

	_prevPerfCount = perfCount;
}
