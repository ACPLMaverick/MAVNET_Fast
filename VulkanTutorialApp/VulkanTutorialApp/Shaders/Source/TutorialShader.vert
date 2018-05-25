#version 450
#extension GL_KHR_vulkan_glsl : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;
out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(inPosition, 1.0f);
	fragColor = inColor;
}