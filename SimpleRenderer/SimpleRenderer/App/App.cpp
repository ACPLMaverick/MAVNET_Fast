#include "App.h"

#include "Scene.h"
#include "Camera.h"

App::App()
	: _window(nullptr)
	, _scene(nullptr)
	, _bInitialized(false)
	, _bRunning(false)
{
}


App::~App()
{
}

App& App::GetInstance()
{
	static App app;
	return app;
}

void App::Run()
{
	Init();
	MainLoop();
	Shutdown();
}

void App::Stop()
{
	_bRunning = false;
}

void App::Init()
{
	SR_Assert(!_bInitialized);

	SR_AssertGlfw(glfwInit());

	// Window setup.

	const uint32_t width = 640;
	const uint32_t height = 480;
	glfwWindowHint(GLFW_RESIZABLE, (int)false);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	_window = glfwCreateWindow(width, height, "SimpleRenderer", nullptr, nullptr);
	SR_Assert(_window != nullptr);
	glfwMakeContextCurrent(_window);

	SR_AssertGlew(glewInit());


	// Viewport setup.

	glViewport(0, 0, width, height);

	// Buffer setup.

	// Render state setup.

	glClearColor(0.3f, 0.4f, 0.3f, 0.0f);
	glPointSize(5.0f);
	glLineWidth(2.5f);


	// Components initialize.
	_timer.Init();
	_input.Init();

	// Scene initialize.
	_scene = PickScene();
	_scene->Init();

	_bInitialized = true;
	_bRunning = true;
	SR_PrintL("App initialized");
}

void App::Shutdown()
{
	SR_Assert(_bInitialized);
	SR_Assert(!_bRunning);

	SR_CleanObject(_scene);

	_input.Shutdown();
	_timer.Shutdown();

	glfwDestroyWindow(_window);
	glfwTerminate();
	
	_bInitialized = false;
	SR_PrintL("App cleaned up.");
}

void App::MainLoop()
{
	while (_bRunning)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		_timer.Update();
		_input.Update();

		if (HasScene())
		{
			_scene->Update();
			_scene->Draw();
		}

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}
}

#include "SceneBezier.h"

Scene * App::PickScene()
{
	return new SceneBezier();
}

Camera & App::GetCam()
{
	return GetInstance().GetScene().GetCamera();
}