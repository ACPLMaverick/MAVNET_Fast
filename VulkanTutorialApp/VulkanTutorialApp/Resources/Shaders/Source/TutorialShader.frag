#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec3 inWVPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUv;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main()
{
	vec4 tex = texture(texSampler, inUv);
	outColor = vec4(tex.xyz * inColor, 1.0f);
}