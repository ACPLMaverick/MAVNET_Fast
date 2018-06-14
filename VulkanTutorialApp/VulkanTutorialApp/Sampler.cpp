#include "Sampler.h"

#include "Core/HelloTriangle.h"

namespace Rendering
{
	Sampler::Sampler()
		: _sampler(VK_NULL_HANDLE)
		, _options(Options())
	{
	}

	Sampler::Sampler(const Sampler & copy)
		: _sampler(copy._sampler)
		, _options(copy._options)
	{
		
	}

	Sampler::~Sampler()
	{
		JE_Assert(_sampler == VK_NULL_HANDLE);
	}

	void Sampler::Initialize(const Options * options)
	{
		JE_Assert(options != nullptr);

		CreateSampler(options);
	}

	void Sampler::Cleanup()
	{
		JE_Assert(_sampler != VK_NULL_HANDLE);

		vkDestroySampler(JE_GetRenderer()->GetDevice(), _sampler, JE_GetRenderer()->GetAllocatorPtr());
		_sampler = VK_NULL_HANDLE;
	}

	void Sampler::CreateSampler(const Options * options)
	{
		JE_Assert(_sampler == VK_NULL_HANDLE);
		JE_Assert(options->OptMaxMipmapLevel > 0);

		const VkBorderColor borderColor = UtilBorderColorToVk(options->OptBorderColor);
		const VkSamplerAddressMode addrMode = UtilAddressModeToVk(options->OptAddressMode);
		const VkFilter filter = UtilFilterModeToVkFilter(options->OptFilterMode);
		const VkSamplerMipmapMode mipMode = UtilFilterModeToVkMipmapMode(options->OptMipmapFilterMode);

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = filter;
		samplerInfo.minFilter = filter;
		samplerInfo.addressModeU = addrMode;
		samplerInfo.addressModeV = addrMode;
		samplerInfo.addressModeW = addrMode;
		samplerInfo.anisotropyEnable = options->OptAnisotropyLevel > 1;
		samplerInfo.maxAnisotropy = static_cast<float>(options->OptAnisotropyLevel);
		samplerInfo.borderColor = borderColor;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = mipMode;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(options->OptMaxMipmapLevel);

		JE_AssertThrowVkResult(vkCreateSampler(JE_GetRenderer()->GetDevice(), &samplerInfo, JE_GetRenderer()->GetAllocatorPtr(), &_sampler));
	}
}