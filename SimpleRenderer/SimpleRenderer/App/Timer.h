#pragma once

class App;

class Timer
{
public:

	void Init();
	void Shutdown();
	void Update();

	float GetTotalTime() const { return (float)_totalTime; }
	float GetDeltaTime() const { return (float)_deltaTime; }

private:

	Timer();
	~Timer();

	LARGE_INTEGER _prevPerfCount;
	double _totalTime;
	double _deltaTime;

	friend class App;
};