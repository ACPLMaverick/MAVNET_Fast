#version 440 core
#extension GL_ARB_explicit_uniform_location : require

out vec4 outColor;

layout (location = 1) uniform vec4 uniColor;

void main()
{
	outColor = vec4(uniColor.xyz, 1.0f);
}