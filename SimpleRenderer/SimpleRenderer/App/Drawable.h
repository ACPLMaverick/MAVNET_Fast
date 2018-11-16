#pragma once

class Shader;

class Drawable
{
public:

	enum class DrawMode : uint8_t
	{
		Points,
		Lines,
		Triangles,
		ENUM_SIZE
	};

	class Params
	{
	public:
		Shader* Shd = nullptr;

		Params() {}
		virtual ~Params() {}
	};

	Drawable();
	virtual ~Drawable();

	virtual void Init(Params* params);
	virtual void Shutdown();
	virtual void Update() = 0;
	virtual void Draw() const = 0;

	Params& GetParams(bool bWillChange = true)
	{
		if (bWillChange)
			_bParamsChanged = true;

		return *_params;
	}

protected:

	static uint16_t DrawModeToGL(DrawMode drawMode) { return DrawModeToGLArray[(size_t)drawMode]; }


	static const uint16_t DrawModeToGLArray[(size_t)DrawMode::ENUM_SIZE];

	Params* _params;
	bool _bParamsChanged;
};

