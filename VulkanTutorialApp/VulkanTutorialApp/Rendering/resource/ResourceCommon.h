#pragma once

namespace Rendering
{
	namespace ResourceCommon
	{
		JE_EnumBegin(Type)
			Unknown
			, UniformBuffer
			, Texture2D

			// Non-shader input resources go here

			, Shader
			, Material
		JE_EnumEnd()

		static const size_t Type_ShaderResourceMin = 1;
		static const size_t Type_ShaderResourceMax = 2;
		static const size_t Type_ShaderResourceNum = Type_ShaderResourceMax - Type_ShaderResourceMin + 1;

		extern VkDescriptorType TypeToDescriptorType(ResourceCommon::Type type);
	}
}