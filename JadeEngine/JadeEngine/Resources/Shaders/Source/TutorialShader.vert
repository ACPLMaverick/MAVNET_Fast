#version 450
#extension GL_KHR_vulkan_glsl : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUv;

layout(set = 0, binding = 0) uniform UniformBufferObject
{
	mat4 MVP;
	mat4 MV;
	mat4 MVInverseTranspose;
} ubo;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outWVPositionDepth;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec2 outUv;
out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = ubo.MVP * vec4(inPosition, 1.0f);
	outColor = inColor;
	outWVPositionDepth.xyz = (ubo.MV * vec4(inPosition, 1.0f)).xyz;
	outWVPositionDepth.w = gl_Position.z / gl_Position.w;
	outNormal = normalize((ubo.MVInverseTranspose * vec4(inNormal, 0.0f)).xyz);
	outUv = inUv;
}