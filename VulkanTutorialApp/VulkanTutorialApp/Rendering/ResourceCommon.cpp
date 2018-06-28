#include "ResourceCommon.h"

namespace Rendering
{
	namespace ResourceCommon
	{
		VkDescriptorType TypeToDescriptorType(ResourceCommon::Type type)
		{
			switch (type)
			{
			case ResourceCommon::Type::UniformBuffer:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
			case ResourceCommon::Type::Texture2D:
				return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				break;
			default:
				JE_Assert(false);
				return VK_DESCRIPTOR_TYPE_END_RANGE;
				break;
			}
		}
	}
}