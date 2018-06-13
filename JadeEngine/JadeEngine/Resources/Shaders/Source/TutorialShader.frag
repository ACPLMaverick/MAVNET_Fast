#version 450
#extension GL_KHR_vulkan_glsl : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec4 inWVPositionDepth;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUv;

layout(binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform PushConsts01
{
	vec3 LightColor;
	vec3 LightDirectionV;
	float FogDepthNear;
	vec3 FogColor;
	float FogDepthFar;
} pushConsts;

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 objColor = texture(texSampler, inUv).rgb;
	objColor *= inColor.rgb;

	float depth = inWVPositionDepth.w;
	float fogCoeff = smoothstep(pushConsts.FogDepthNear, pushConsts.FogDepthFar, depth);
	outColor.rgb = mix(objColor, pushConsts.FogColor, fogCoeff);
	outColor.a = 1.0f;
}