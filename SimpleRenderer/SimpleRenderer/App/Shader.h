#pragma once
class Shader
{
public:

	enum class Module : uint8_t
	{
		Vertex,
		Pixel,
		ENUM_SIZE
	};

	Shader();
	~Shader();

	void Init(const std::string& name);
	void Shutdown();

	GLuint GetProgram() const { return _program; }

	void Use();
	void SetUniformMVP(const glm::mat4& mvp);
	void SetUniformColor(const glm::vec4& color);

private:

	static const std::string RESOURCE_PATH_BASE;
	static const std::string SHADER_PATH_BASE;

	static const std::string SUFFIXES[(size_t)Module::ENUM_SIZE];
	static const uint16_t TYPES[(size_t)Module::ENUM_SIZE];

	void LoadShader(const std::string& name, GLuint* modules, size_t moduleIndex);
	size_t LoadFile(const std::string& path, uint8_t*& outData);
	void UnloadFile(uint8_t* data);

	GLuint _program;
	bool _bUsed;
};

