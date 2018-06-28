#pragma once

namespace Rendering
{
	namespace ResourceCommon
	{
		JE_EnumBegin(Type)
			Unknown,
			UniformBuffer,
			Texture2D
		JE_EnumEnd()

		extern VkDescriptorType TypeToDescriptorType(ResourceCommon::Type type);
	}
}