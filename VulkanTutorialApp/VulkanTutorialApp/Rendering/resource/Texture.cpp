#include "Texture.h"

#include "Core/HelloTriangle.h"
#include "Rendering/sampler/ManagerSampler.h"

namespace Rendering
{
	Texture::Texture()
		: Resource()
		, _dataGpu(VK_NULL_HANDLE)
		, _image(VK_NULL_HANDLE)
		, _view(VK_NULL_HANDLE)
		, _sampler(nullptr)
	{
		_type = ResourceCommon::Type::Texture2D;
	}


	Texture::~Texture()
	{
		JE_Assert(_info.Data == nullptr);
		JE_Assert(_sampler == nullptr);
		JE_Assert(_dataGpu == VK_NULL_HANDLE);
		JE_Assert(_view == VK_NULL_HANDLE);
		JE_Assert(_image == VK_NULL_HANDLE);
	}

	JE_Inline bool Texture::CanKeepContentOnResize() const
	{
		// Not supported for depth/stencil format for now...
		return _info.bTransferable && !((uint32_t)ObtainImageUsage() & (uint32_t)VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	void Texture::Create(const CreateOptions* createOptions)
	{
		JE_Assert(createOptions);

		_info = createOptions->CreationInfo;
		if (createOptions->bGenerateMips && _info.MipCount == 1)
		{
			CalculateMipCount(true);
		}

		InitializeCommon(const_cast<Sampler::Options*>(&createOptions->SamplerOptions), createOptions->bGenerateMips, createOptions->bCPUImmutable);

		if (createOptions->bClearOnCreate)
		{
			ClearWithFixedValue(createOptions->ClearValuesNormalized);
		}
	}

	void Texture::Load(const std::string& loadPath, const LoadOptions * loadOptions)
	{
		JE_Assert(loadOptions);

		_info.Format = loadOptions->DesiredFormat;
		LoadData(loadPath, loadOptions);

		InitializeCommon(const_cast<Sampler::Options*>(&loadOptions->SamplerOptions), loadOptions->bGenerateMips, loadOptions->bCPUImmutable);
	}

	void Texture::Cleanup()
	{
		if (!IsReadOnly())
		{
			CleanupData();
		}

		vkDestroyImageView(JE_GetRenderer()->GetDevice(), _view, JE_GetRenderer()->GetAllocatorPtr());
		_view = VK_NULL_HANDLE;

		if (CanDestroyImage())
		{
			vkDestroyImage(JE_GetRenderer()->GetDevice(), _image, JE_GetRenderer()->GetAllocatorPtr());
		}
		_image = VK_NULL_HANDLE;

		if (_dataGpu)
		{
			vkFreeMemory(JE_GetRenderer()->GetDevice(), _dataGpu, JE_GetRenderer()->GetAllocatorPtr());
			_dataGpu = VK_NULL_HANDLE;
		}

		_sampler = nullptr;
	}

	void Texture::Resize(const ResizeInfo* resizeInfo)
	{
		JE_Assert(resizeInfo);
		JE_Assert(CanDestroyImage());
		JE_Assert(IsReadOnly());		// For now let it be like this...
		if (resizeInfo->bKeepContents)
		{
			JE_Assert(CanKeepContentOnResize());
		}

		const bool bCanKeepContents = resizeInfo->bKeepContents && CanKeepContentOnResize();

		if (resizeInfo->Width == _info.Width && resizeInfo->Height == _info.Height)
		{
			return;
		}

		VkImageAspectFlagBits aspect = ObtainImageAspect();
		VkImageLayout destLayout = ObtainDestLayout();
		VkImageUsageFlagBits usage = ObtainImageUsage();

		if (bCanKeepContents)
		{
			TransitionImageLayout(&_info, _image, aspect, destLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		}

		uint32_t oldWidth = _info.Width;
		uint32_t oldHeight = _info.Height;
		_info.Width = resizeInfo->Width;
		_info.Height = resizeInfo->Height;

		VkDeviceMemory newMemory = VK_NULL_HANDLE;
		VkImage newImage = VK_NULL_HANDLE;

		// Copy to new image.
		if(bCanKeepContents)
		{
			CreateImage(&_info, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, newImage, newMemory);
			TransitionImageLayout(&_info, newImage, aspect, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			VkCommandBuffer cmd = JE_GetRenderer()->BeginSingleTimeCommands();

			JE_Assert(_info.MipCount);
			for (uint32_t i = 0; i < _info.MipCount; ++i)
			{
				int32_t currOldWidth = std::max<int32_t>((int32_t)oldWidth >> i, 1);
				int32_t currOldHeight = std::max<int32_t>((int32_t)oldHeight >> i, 1);

				int32_t currNewWidth = std::max<int32_t>((int32_t)_info.Width >> i, 1);
				int32_t currNewHeight = std::max<int32_t>((int32_t)_info.Height >> i, 1);

				VkImageBlit blit = {};

				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { currOldWidth, currOldHeight, 1 };
				blit.srcSubresource.aspectMask = aspect;
				blit.srcSubresource.mipLevel = i;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = 1;

				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { currNewWidth, currNewHeight, 1 };
				blit.dstSubresource.aspectMask = aspect;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = 1;

				VkFilter blitFilter = ((uint32_t)usage & (uint32_t)VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;

				vkCmdBlitImage(cmd, _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, newImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, blitFilter);
			}

			JE_GetRenderer()->EndSingleTimeCommands(cmd);

			// Move to destination layout after blit is finished.
			TransitionImageLayout(&_info, newImage, aspect, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, destLayout);
		}
		else
		{
			CreateImage(&_info, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, newImage, newMemory);
			TransitionImageLayout(&_info, newImage, aspect, VK_IMAGE_LAYOUT_UNDEFINED, destLayout);
		}


		// Purge the old image and reassign to new.
		vkDestroyImageView(JE_GetRenderer()->GetDevice(), _view, JE_GetRenderer()->GetAllocatorPtr());
		if (_dataGpu)
		{
			vkFreeMemory(JE_GetRenderer()->GetDevice(), _dataGpu, JE_GetRenderer()->GetAllocatorPtr());
			_dataGpu = VK_NULL_HANDLE;
		}
		vkDestroyImage(JE_GetRenderer()->GetDevice(), _image, JE_GetRenderer()->GetAllocatorPtr());

		_dataGpu = newMemory;
		_image = newImage;
		CreateImageView(aspect);
	}

	int32_t Texture::GetDesiredChannelsFromFormat(VkFormat format)
	{
		// TODO: Implement.
		return 4;
	}

	void Texture::CreateImage(const ::Rendering::Texture::Info* texInfo, VkImageTiling tiling, VkImageLayout initialLayout, VkImageUsageFlags usage, VkMemoryPropertyFlags memProperties, VkImage& outImage, VkDeviceMemory& outMemory)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(texInfo->Width);
		imageInfo.extent.height = static_cast<uint32_t>(texInfo->Height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = static_cast<uint32_t>(texInfo->MipCount);
		imageInfo.arrayLayers = 1;
		imageInfo.format = static_cast<VkFormat>(texInfo->Format);
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = initialLayout;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;

		JE_AssertThrowVkResult(vkCreateImage(JE_GetRenderer()->GetDevice(), &imageInfo, JE_GetRenderer()->GetAllocatorPtr(), &outImage));
		JE_Assert(outImage);

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(JE_GetRenderer()->GetDevice(), outImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = JE_GetRenderer()->FindMemoryType(memRequirements.memoryTypeBits, memProperties);

		JE_AssertThrowVkResult(vkAllocateMemory(JE_GetRenderer()->GetDevice(), &allocInfo, JE_GetRenderer()->GetAllocatorPtr(), &outMemory));

		JE_AssertThrowVkResult(vkBindImageMemory(JE_GetRenderer()->GetDevice(), outImage, outMemory, 0));
	}

	void Texture::CopyBufferToImage(VkBuffer buffer, VkImage image, const ::Rendering::Texture::Info* texInfo)
	{
		VkCommandBuffer commandBuffer = JE_GetRenderer()->BeginSingleTimeCommands();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent =
		{
			static_cast<uint32_t>(texInfo->Width),
			static_cast<uint32_t>(texInfo->Height),
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		JE_GetRenderer()->EndSingleTimeCommands(commandBuffer);
	}

	void Texture::InitializeCommon(Sampler::Options* samplerOptions, bool bGenerateMips, bool bCPUImmutable)
	{
		VkImageAspectFlagBits aspect = ObtainImageAspect();
		VkImageLayout destLayout = ObtainDestLayout();
		VkImageUsageFlagBits usage = ObtainImageUsage();

		CreateImage(destLayout, aspect, usage, bGenerateMips);
		CreateImageView(aspect);

		if (bGenerateMips)
		{
			GenerateMipmaps(destLayout, aspect);
		}

		AssignSampler(samplerOptions);

		if (bCPUImmutable)
		{
			CleanupData();
		}
	}

	void Texture::LoadData(const std::string& textureName, const LoadOptions * loadOptions)
	{
		int32_t width, height, chnls;
		int32_t desiredChannels = GetDesiredChannelsFromFormat(loadOptions->DesiredFormat);

		_info.Data = stbi_load
		(
			(::Core::HelloTriangle::RESOURCE_PATH + "Textures\\Source\\" + textureName).c_str(),
			&width,
			&height,
			&chnls,
			desiredChannels
		);

		// TODO: Load Compressed texture based on extension.
		uint8_t finalMipCount = 1;

		JE_Assert(_info.Data != nullptr);

		_info.Width = width;
		_info.Height = height;
		_info.Channels = desiredChannels;
		_info.SizeBytes = width * height * _info.Channels;
		_info.bAllocatedByStbi = true;
		CalculateMipCount(loadOptions->bGenerateMips && finalMipCount == 1);
	}

	void Texture::CalculateMipCount(bool bGenerateMips)
	{
		if (bGenerateMips)
		{
			_info.MipCount = static_cast<uint8_t>(std::floor(std::log2(std::max(_info.Width, _info.Height)))) + 1;
		}
		else
		{
			_info.MipCount = 1;
		}
	}

	void Texture::CreateImage(VkImageLayout destLayout, VkImageAspectFlagBits imageAspect, VkImageUsageFlagBits imageUsage, bool bGenerateMips)
	{
		VkDeviceSize imageSize = _info.Width * _info.Height * _info.Channels;

		if (_info.Data)
		{
			// Copy texture data from CPU to GPU after creation.

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;

			JE_GetRenderer()->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			JE_GetRenderer()->CopyBuffer_CPU_GPU(reinterpret_cast<void*>(_info.Data), stagingBufferMemory, imageSize);

			CreateImage(GetInfo(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | imageUsage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _image, _dataGpu);

			TransitionImageLayout(GetInfo(), _image, imageAspect, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			CopyBufferToImage(stagingBuffer, _image, GetInfo());

			if (!bGenerateMips)
			{
				TransitionImageLayout(GetInfo(), _image, imageAspect, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, destLayout);
			}

			vkDestroyBuffer(JE_GetRenderer()->GetDevice(), stagingBuffer, JE_GetRenderer()->GetAllocatorPtr());
			vkFreeMemory(JE_GetRenderer()->GetDevice(), stagingBufferMemory, JE_GetRenderer()->GetAllocatorPtr());
		}
		else
		{
			// Create texture already in deep GPU memory.
			CreateImage(GetInfo(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, imageUsage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _image, _dataGpu);
			TransitionImageLayout(GetInfo(), _image, imageAspect, VK_IMAGE_LAYOUT_UNDEFINED, destLayout);
		}
	}

	VkImageView Texture::CreateImageView(const Texture::Info* texInfo, VkImage image, VkImageAspectFlagBits flags)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = static_cast<VkFormat>(texInfo->Format);
		viewInfo.subresourceRange.aspectMask = flags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = static_cast<uint32_t>(texInfo->MipCount);
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		VkImageView imageView;
		JE_AssertThrowVkResult(vkCreateImageView(JE_GetRenderer()->GetDevice(), &viewInfo, JE_GetRenderer()->GetAllocatorPtr(), &imageView));
		return imageView;
	}

	void Texture::TransitionImageLayout(const ::Rendering::Texture::Info* texInfo, VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = JE_GetRenderer()->BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspectFlags;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = static_cast<uint32_t>(texInfo->MipCount);
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage, destinationStage;
		if (
			oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
			&& newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (
			oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			&& newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (
			oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			&& newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (
			oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			&& newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (
			oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
			&& newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (
			oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			&& newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			)
		{
			barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else
		{
			JE_AssertThrow(false, "Unsupported layout transition!");
		}

		vkCmdPipelineBarrier
		(
			commandBuffer,
			sourceStage,
			destinationStage,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);

		JE_GetRenderer()->EndSingleTimeCommands(commandBuffer);
	}

	void Texture::CreateImageView(VkImageAspectFlagBits imageAspect)
	{
		_view = CreateImageView(GetInfo(), _image, imageAspect);
	}

	void Texture::AssignSampler(Sampler::Options* samplerOptions)
	{
		JE_Assert(samplerOptions);
		if (samplerOptions->OptMaxMipmapLevel == 0)
		{
			samplerOptions->OptMaxMipmapLevel = _info.MipCount;
		}

		_sampler = Core::HelloTriangle::GetInstance()->GetManagerSampler()->Get(samplerOptions);
	}

	void Texture::CleanupData()
	{
		if (_info.Data == nullptr)
			return;

		if (_info.bAllocatedByStbi)
		{
			stbi_image_free(_info.Data);
		}
		else
		{
			free(_info.Data);
		}

		_info.Data = nullptr;
	}

	void Texture::GenerateMipmaps(VkImageLayout destLayout, VkImageAspectFlagBits imageAspect)
	{
		VkCommandBuffer commandBuffer = JE_GetRenderer()->BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = _image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = imageAspect;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = _info.Width;
		int32_t mipHeight = _info.Height;

		for (uint32_t i = 1; i < static_cast<uint32_t>(_info.MipCount); ++i)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier
			(
				commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&barrier
			);

			const int32_t nextMipWidth = mipWidth / 2;
			const int32_t nextMipHeight = mipHeight / 2;

			VkImageBlit blit = {};

			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = imageAspect;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;

			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { nextMipWidth, nextMipHeight, 1 };
			blit.dstSubresource.aspectMask = imageAspect;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer, _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier
			(
				commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&barrier
			);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = static_cast<uint32_t>(_info.MipCount) - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = destLayout;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier
		(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);

		JE_GetRenderer()->EndSingleTimeCommands(commandBuffer);
	}

	void Texture::ClearWithFixedValue(const glm::vec4& clearValuesNormalized)
	{
		VkCommandBuffer cmd = JE_GetRenderer()->BeginSingleTimeCommands();

		VkClearColorValue clearColor;
		clearColor.float32[0] = clearValuesNormalized.r;
		clearColor.float32[1] = clearValuesNormalized.g;
		clearColor.float32[2] = clearValuesNormalized.b;
		clearColor.float32[3] = clearValuesNormalized.a;

		vkCmdClearColorImage(cmd, _image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &clearColor, 0, NULL);

		JE_GetRenderer()->EndSingleTimeCommands(cmd);
	}

	VkImageAspectFlagBits Texture::ObtainImageAspect() const
	{
		return VK_IMAGE_ASPECT_COLOR_BIT;
	}

	VkImageLayout Texture::ObtainDestLayout() const
	{
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	VkImageUsageFlagBits Texture::ObtainImageUsage() const
	{
		return VK_IMAGE_USAGE_SAMPLED_BIT;
	}

	bool Texture::CanDestroyImage() const
	{
		return true;
	}

}