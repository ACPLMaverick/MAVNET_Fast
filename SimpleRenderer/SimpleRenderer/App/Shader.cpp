#include "Shader.h"

#include "App.h"
#include "Scene.h"

#include <fstream>

const std::string Shader::RESOURCE_PATH_BASE = "Resources\\";
const std::string Shader::SHADER_PATH_BASE = Shader::RESOURCE_PATH_BASE + "Shaders\\";

const std::string Shader::SUFFIXES[(size_t)Module::ENUM_SIZE] = 
{
	"_VS.glsl",
	"_PS.glsl"
};

const uint16_t Shader::TYPES[(size_t)Module::ENUM_SIZE] =
{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER
};

Shader::Shader()
	: _program(0)
	, _bUsed(false)
{
}


Shader::~Shader()
{
}

void Shader::Init(const std::string& name)
{
	GLuint modules[(size_t)Module::ENUM_SIZE];

	_program = glCreateProgram();

	for (size_t i = 0; i < (size_t)Module::ENUM_SIZE; ++i)
	{
		LoadShader(name, modules, i);
		glAttachShader(_program, modules[i]);
	}

	glLinkProgramARB(_program);
	GLint success;
	glGetProgramiv(_program, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		static const GLsizei BUF_SIZ = 512;
		char infoLog[BUF_SIZ] = {};
		glGetProgramInfoLog(_program, BUF_SIZ, nullptr, infoLog);
		SR_Assert(false);
	}

	for (size_t i = 0; i < (size_t)Module::ENUM_SIZE; ++i)
	{
		glDeleteShader(modules[i]);
	}
}

void Shader::Shutdown()
{
	glDeleteProgram(_program);
	_program = 0;
}

void Shader::Use()
{
	if (_bUsed)
		return;

	const std::unordered_map<std::string, Shader*> shaders = App::GetInstance().GetScene().GetShaders();

	for (std::pair<std::string, Shader*> pair : shaders)
	{
		if (pair.second != this)
		{
			pair.second->_bUsed = false;
		}
	}

	_bUsed = true;

	glUseProgram(_program);
}

void Shader::SetUniformMVP(const glm::mat4 & mvp)
{
	glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);
}

void Shader::SetUniformColor(const glm::vec4 & color)
{
	glUniform4fv(1, 1, &color[0]);
}

void Shader::LoadShader(const std::string & name, GLuint* modules, size_t moduleIndex)
{
	// Load shader from file.

	const std::string path = SHADER_PATH_BASE + name + SUFFIXES[moduleIndex];
	uint8_t* data;
	size_t dataSize = LoadFile(path, data);

	modules[moduleIndex] = glCreateShader(TYPES[moduleIndex]);
	glShaderSource(modules[moduleIndex], 1, (GLchar**)&data, (GLint*)&dataSize);
	glCompileShaderARB(modules[moduleIndex]);

	UnloadFile(data);

	// Print error message if needed.

	GLint success;
	glGetShaderiv(modules[moduleIndex], GL_COMPILE_STATUS, &success);
	if (!success)
	{
		static const GLsizei BUF_SIZ = 512;
		char infoLog[BUF_SIZ] = {};
		glGetShaderInfoLog(modules[moduleIndex], BUF_SIZ, nullptr, infoLog);
		SR_Assert(false);
	}
}

size_t Shader::LoadFile(const std::string& path, uint8_t*& outData)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		return false;

	// (ate) The advantage of starting to read at the end of the file is that we can use the read position to determine the size of the file and allocate a buffer

	size_t fileSize = static_cast<size_t>(file.tellg());
	outData = new uint8_t[fileSize];

	file.seekg(0);
	file.read(reinterpret_cast<char*>(outData), fileSize);

	file.close();

	return fileSize;
}

void Shader::UnloadFile(uint8_t * data)
{
	delete[] data;
}
