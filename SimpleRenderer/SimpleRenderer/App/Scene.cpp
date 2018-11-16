#include "Scene.h"

#include "App.h"

#include "Drawable.h"
#include "Shader.h"
#include "Camera.h"

Scene::Scene()
	: _playerDrawable(nullptr)
{
}


Scene::~Scene()
{
}

void Scene::Init()
{
	InitResources();
	
	InitCamera();
	SR_Assert(_camera != nullptr);

	InitDrawables();

	SR_Assert(_playerDrawable != nullptr);
}

void Scene::Shutdown()
{
	for (Drawable* drawable : _drawables)
	{
		drawable->Shutdown();
		delete drawable;
	}
	_drawables.clear();
	_playerDrawable = nullptr;

	_camera->Shutdown();
	delete _camera;
	_camera = nullptr;

	for (std::pair<std::string, Shader*> shaderPair : _shaders)
	{
		shaderPair.second->Shutdown();
		delete shaderPair.second;
	}
	_shaders.clear();
}

void Scene::Update()
{
	if (App::GetInstance().GetInput().IsKeyDown(GLFW_KEY_ESCAPE))
	{
		if (App::GetInstance().IsRunning())
		{
			App::GetInstance().Stop();
		}
	}
	else
	{
		for (Drawable* drawable : _drawables)
		{
			drawable->Update();
		}

		UpdateLogic();
	}
}

void Scene::Draw() const
{
	for (Drawable* drawable : _drawables)
	{
		drawable->Draw();
	}
}
