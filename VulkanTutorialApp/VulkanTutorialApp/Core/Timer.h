#pragma once

namespace Core
{
	class Timer
	{
	public:

		Timer();
		~Timer();

		void UpdatePerFrame();

		JE_Inline float GetDt() const { return _dt; }
		JE_Inline float GetFPS() const { return _fps; }
		JE_Inline float GetElapsed() const { return _elapsed; }
		JE_Inline uint64_t GetDtTicks() const { return _currentTicks - _prevTicks; }
		JE_Inline uint64_t GetElapsedTicks() const { return _currentTicks - _startTicks; }

	protected:

		static uint64_t GetTicks();
		static double GetFrequency();

		uint64_t _startTicks;
		uint64_t _prevTicks;
		uint64_t _currentTicks;

		float _dt;
		float _fps;
		float _elapsed;
	};
}