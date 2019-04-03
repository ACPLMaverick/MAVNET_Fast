#include "Timer.h"


namespace Core
{
	Timer::Timer()
		: _startTicks(GetTicks())
		, _prevTicks(_startTicks)
		, _currentTicks(_startTicks)
	{
		// Perform first update to properly initialize these values.
		UpdatePerFrame();
	}


	Timer::~Timer()
	{
	}

	void Timer::UpdatePerFrame()
	{
		_prevTicks = _currentTicks;
		_currentTicks = GetTicks();

		const double freq = GetFrequency();
		_dt = (float)((double)(_currentTicks - _prevTicks) / freq);
		_elapsed = (float)((double)(_currentTicks - _startTicks) / freq);

		_fps = _dt > std::numeric_limits<float>::epsilon() ? 1.0f / _dt : 0.0f;
	}

}