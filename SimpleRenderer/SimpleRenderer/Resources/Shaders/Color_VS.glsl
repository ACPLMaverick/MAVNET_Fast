#version 440 core

layout (location = 0) in vec3 inPosition;

layout (location = 0) uniform mat4 uniMVP;

void main()
{
	gl_Position = uniMVP * vec4(inPosition, 1.0f);
}