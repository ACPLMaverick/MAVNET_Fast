#if PLATFORM_WINDOWS

#include "Timer.h"

#include <Windows.h>

namespace Core
{
	uint64_t Timer::GetTicks()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}

	double Timer::GetFrequency()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		return (double)li.QuadPart;
	}
}

#endif