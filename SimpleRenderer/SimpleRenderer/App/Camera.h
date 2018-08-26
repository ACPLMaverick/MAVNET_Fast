#pragma once
class Camera
{
public:

	enum class Mode : uint8_t
	{
		Ortho,
		Perspective,
		ENUM_SIZE
	};

	struct Params
	{
		glm::vec3 Position = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 Direction = glm::vec3(0.0f, 0.0f, 1.0f);
		Mode CamMode = Mode::Ortho;
		float Width = 640;
		float Height = 480;
		float PerspFov = 90.0f;
		float OrthoUnitHeight = 20.0f;
		float NearPlane = 0.1f;
		float FarPlane = 1000.0f;
	};

	Camera();
	~Camera();

	void Init(const Params& params);
	void Shutdown();

	const glm::mat4& GetVP() const { return _matVP; }
	const Params& GetParams() const { return _params; }

private:

	void BuildView();
	void BuildProj();
	void BuildViewProj();

	glm::mat4 _matVP = glm::mat4(1.0f);
	glm::mat4 _matV = glm::mat4(1.0f);
	glm::mat4 _matP = glm::mat4(1.0f);

	Params _params;
};

