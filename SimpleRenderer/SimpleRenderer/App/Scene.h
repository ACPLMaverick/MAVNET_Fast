#pragma once

class Drawable;
class Shader;
class Camera;

class Scene
{
public:

	Scene();
	virtual ~Scene();

	void Init();
	void Shutdown();
	void Update();
	void Draw() const;

	Camera& GetCamera() { SR_Assert(_camera != nullptr); return *_camera; }
	const std::unordered_map<std::string, Shader*>& GetShaders() { return _shaders; }

protected:

	virtual void InitResources() = 0;
	virtual void InitCamera() = 0;
	virtual void InitDrawables() = 0;
	virtual void UpdateLogic() = 0;

	std::vector<Drawable*> _drawables;
	Drawable* _playerDrawable;

	std::unordered_map<std::string, Shader*> _shaders;

	Camera* _camera;
};

