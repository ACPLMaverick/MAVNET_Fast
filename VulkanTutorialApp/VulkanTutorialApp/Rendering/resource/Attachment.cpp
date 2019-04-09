#include "Attachment.h"

#include "Core/HelloTriangle.h"

namespace Rendering
{

	void Attachment::CreateAttachmentsFromSwapchain(std::vector<Attachment*>& outAttachments)
	{
		outAttachments.clear();
		// The implementation is allowed to create more images, which is why we need to explicitly query the amount again.
		uint32_t imageCount;
		JE_AssertThrowVkResult(vkGetSwapchainImagesKHR(JE_GetRenderer()->GetDevice(), JE_GetRenderer()->GetSwapChain(), &imageCount, nullptr));
		outAttachments.resize(imageCount);

		std::vector<VkImage> images(imageCount);
		JE_AssertThrowVkResult(vkGetSwapchainImagesKHR(JE_GetRenderer()->GetDevice(), JE_GetRenderer()->GetSwapChain(), &imageCount, images.data()));

		VkExtent2D swapChainExtent = JE_GetRenderer()->GetSwapChainExtent();
		VkFormat swapChainFormat = JE_GetRenderer()->GetSwapChainFormat();

		CreateOptions opt;
		opt.bClearOnCreate = false;
		opt.bClearOnLoad = true;
		opt.bGenerateMips = false;
		opt.bIsTransferable = false;
		opt.bCPUImmutable = true;
		opt.SamplerOptions = Sampler::Options();
		opt.CreationInfo.Width = swapChainExtent.width;
		opt.CreationInfo.Height = swapChainExtent.height;
		opt.CreationInfo.Channels = GetDesiredChannelsFromFormat(swapChainFormat);
		opt.CreationInfo.SizeBytes = opt.CreationInfo.Width * opt.CreationInfo.Height * opt.CreationInfo.Channels;
		opt.CreationInfo.Data = nullptr;
		opt.CreationInfo.Format = swapChainFormat;
		opt.CreationInfo.MipCount = 1;
		opt.CreationInfo.MyMultisamplingMode = RenderState::MultisamplingMode::None;
		
		for (uint32_t i = 0; i < imageCount; ++i)
		{
			Attachment* attachment = new Attachment();
			attachment->CreateFromSwapChainImage(&opt, images[i]);
			outAttachments[i] = attachment;
		}
	}

	void Attachment::Create(const CreateOptions* createOptions)
	{
		BuildAttachDesc(createOptions);
		Texture::Create(createOptions);
	}

	void Attachment::Resize(const ResizeInfo* resizeInfo)
	{
		Texture::Resize(resizeInfo);
	}

	void Attachment::ClearWithFixedValue(const glm::vec4& clearValuesNormalized)
	{
		if (_attachDesc.Usage == UsageMode::DepthStencil)
		{
			VkCommandBuffer cmd = JE_GetRenderer()->BeginSingleTimeCommands();

			VkClearDepthStencilValue clearDepth;
			clearDepth.depth = clearValuesNormalized.r;
			clearDepth.stencil = static_cast<uint32_t>(clearValuesNormalized.g);

			vkCmdClearDepthStencilImage(cmd, _image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &clearDepth, 0, NULL);

			JE_GetRenderer()->EndSingleTimeCommands(cmd);
		}
		else
		{
			Texture::ClearWithFixedValue(clearValuesNormalized);
		}
	}

	VkImageAspectFlagBits Attachment::ObtainImageAspect()
	{
		JE_Assert(_attachDesc.Usage != UsageMode::Transferable); // TODO Not implemented.

		switch (_attachDesc.Usage)
		{
		case UsageMode::DepthStencil:
			return VK_IMAGE_ASPECT_DEPTH_BIT;
		default:
			return VK_IMAGE_ASPECT_COLOR_BIT;
		}
	}

	VkImageLayout Attachment::ObtainDestLayout()
	{
		switch (_attachDesc.Usage)
		{
		case UsageMode::DepthStencil:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		default:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	}

	VkImageUsageFlagBits Attachment::ObtainImageUsage()
	{
		VkImageUsageFlagBits baseUsage = Texture::ObtainImageUsage();

		if (_attachDesc.bWriteOnly)
		{
			baseUsage = (VkImageUsageFlagBits)((uint32_t)baseUsage & ~((uint32_t)VK_IMAGE_USAGE_SAMPLED_BIT));
		}

		switch (_attachDesc.Usage)
		{
		case UsageMode::DepthStencil:
			baseUsage = (VkImageUsageFlagBits)((uint32_t)baseUsage | (uint32_t)VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
			break;
		default:
			baseUsage = (VkImageUsageFlagBits)((uint32_t)baseUsage | (uint32_t)VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
			break;
		}

		return baseUsage;
	}

	bool Attachment::CanDestroyImage()
	{
		return !IsSwapChainImage();
	}

	void Attachment::CreateFromSwapChainImage(const CreateOptions* createOptions, VkImage swapChainImage)
	{
		BuildAttachDesc(createOptions);
		_attachDesc.Usage = UsageMode::ColorPresentable;
		_info = createOptions->CreationInfo;
		_image = swapChainImage;
		CreateImageView(ObtainImageAspect());
		JE_Assert(!createOptions->bGenerateMips);
		_sampler = nullptr;
	}

	void Attachment::BuildAttachDesc(const CreateOptions* createOptions)
	{
		VkFormat vkFormat = static_cast<VkFormat>(createOptions->CreationInfo.Format);
		bool bIsDepth = false;
		bool bUsesStencil = false;

		switch (vkFormat)
		{
		case VK_FORMAT_S8_UINT:
			bIsDepth = false;
			bUsesStencil = true;
			break;
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_D32_SFLOAT:
			bIsDepth = true;
			bUsesStencil = false;
			break;
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			bIsDepth = true;
			bUsesStencil = true;
			break;
		default:
			break;
		}

		_attachDesc.Format = static_cast<uint32_t>(vkFormat);
		_attachDesc.MyMultisamplingMode = _info.MyMultisamplingMode;

		if (createOptions->bIsTransferable)
		{
			_attachDesc.Usage = UsageMode::Transferable;
		}
		else if (bIsDepth || bUsesStencil)
		{
			_attachDesc.Usage = UsageMode::DepthStencil;
		}
		else
		{
			_attachDesc.Usage = UsageMode::Color;
		}

		_attachDesc.bClearOnLoad = createOptions->bClearOnLoad;
		_attachDesc.bWriteOnly = createOptions->bWriteOnly;
		_attachDesc.bStore = true;
		_attachDesc.bUseStencil = bUsesStencil;
	}
}
