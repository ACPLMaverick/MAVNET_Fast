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

		extern VkDescriptorType TypeToDescriptorType(ResourceCommon::Type type);
	}
}