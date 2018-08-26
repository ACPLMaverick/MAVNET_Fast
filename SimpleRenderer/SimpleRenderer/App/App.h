#pragma once

#include "Input.h"
#include "Timer.h"

class Scene;
class Camera;

class App
{
public:
	static App& GetInstance();

	void Run();
	void Stop();

	Input& GetInput() { return _input; }
	Timer& GetTimer() { return _timer; }
	Scene& GetScene() { SR_Assert(_scene != nullptr); return *_scene; }

	bool HasScene() { return _scene != nullptr; }

	GLFWwindow* GetWindowPtr() { return _window; }

	bool IsInitialized() const { return _bInitialized; }
	bool IsRunning() const { return _bRunning; }

	// ++ Helper functions

	static float GetDt() { return GetInstance().GetTimer().GetDeltaTime(); }
	static Camera& GetCam();

	// -- Helper functions

private:
	App();
	~App();

	void Init();
	void Shutdown();
	void MainLoop();

	Scene* PickScene();

	Input _input;
	Timer _timer;

	Scene* _scene;

	GLFWwindow* _window;

	bool _bInitialized;
	bool _bRunning;
};

