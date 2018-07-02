#include "DescriptorCommon.h"

namespace Rendering
{
	namespace DescriptorCommon
	{
		VkShaderStageFlagBits ShaderStageToVkShaderStageFlag(ShaderStage stage)
		{
			return static_cast<VkShaderStageFlagBits>(stage);
		}
	}
}