#include "Texture.h"

#include "Core/HelloTriangle.h"
#include "Rendering/sampler/ManagerSampler.h"

namespace Rendering
{
	VkImageView Texture::UtilCreateImageView(const Texture::Info* texInfo, VkImage image, VkImageAspectFlagBits flags)
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

	void Texture::Create(const CreateOptions* createOptions)
	{
		JE_Assert(createOptions);

		_info = createOptions->CreationInfo;
		if (createOptions->bGenerateMips && _info.MipCount == 1)
		{
			CalculateMipCount(true);
		}

		InitializeCommon(const_cast<Sampler::Options*>(&createOptions->SamplerOptions), createOptions->bGenerateMips, createOptions->bReadOnly);

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

		InitializeCommon(const_cast<Sampler::Options*>(&loadOptions->SamplerOptions), loadOptions->bGenerateMips, loadOptions->bReadOnly);
	}

	void Texture::Cleanup()
	{
		if (!IsReadOnly())
		{
			CleanupData();
		}

		vkDestroyImageView(JE_GetRenderer()->GetDevice(), _view, JE_GetRenderer()->GetAllocatorPtr());
		_view = VK_NULL_HANDLE;
		vkDestroyImage(JE_GetRenderer()->GetDevice(), _image, JE_GetRenderer()->GetAllocatorPtr());
		_image = VK_NULL_HANDLE;
		vkFreeMemory(JE_GetRenderer()->GetDevice(), _dataGpu, JE_GetRenderer()->GetAllocatorPtr());
		_dataGpu = VK_NULL_HANDLE;

		_sampler = nullptr;
	}

	void Texture::Resize(const ResizeInfo* resizeInfo)
	{
		JE_TODO();	// TODO
	}

	int32_t Texture::GetDesiredChannelsFromFormat(VkFormat format)
	{
		// TODO: Implement.
		return 4;
	}

	void Texture::InitializeCommon(Sampler::Options* samplerOptions, bool bGenerateMips, bool bReadOnly)
	{
		VkImageAspectFlagBits aspect = ObtainImageAspect();
		VkImageLayout destLayout = ObtainDestLayout();

		CreateImage(destLayout, aspect, bGenerateMips);
		CreateImageView(aspect);

		if (bGenerateMips)
		{
			GenerateMipmaps(destLayout, aspect);
		}

		AssignSampler(samplerOptions);

		if (bReadOnly)
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

	void Texture::CreateImage(VkImageLayout destLayout, VkImageAspectFlagBits imageAspect, bool bGenerateMips)
	{
		VkDeviceSize imageSize = _info.Width * _info.Height * _info.Channels;

		if (_info.Data)
		{
			// Copy texture data from CPU to GPU after creation.

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;

			JE_GetRenderer()->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			JE_GetRenderer()->CopyBuffer_CPU_GPU(reinterpret_cast<void*>(_info.Data), stagingBufferMemory, imageSize);

			JE_GetRenderer()->CreateImage(GetInfo(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _image, _dataGpu);

			JE_GetRenderer()->TransitionImageLayout(GetInfo(), _image, imageAspect, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			JE_GetRenderer()->CopyBufferToImage(stagingBuffer, _image, GetInfo());

			if (!bGenerateMips)
			{
				JE_GetRenderer()->TransitionImageLayout(GetInfo(), _image, imageAspect, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, destLayout);
			}

			vkDestroyBuffer(JE_GetRenderer()->GetDevice(), stagingBuffer, JE_GetRenderer()->GetAllocatorPtr());
			vkFreeMemory(JE_GetRenderer()->GetDevice(), stagingBufferMemory, JE_GetRenderer()->GetAllocatorPtr());
		}
		else
		{
			// Create texture already in deep GPU memory.
		}
	}

	void Texture::CreateImageView(VkImageAspectFlagBits imageAspect)
	{
		_view = UtilCreateImageView(GetInfo(), _image, imageAspect);
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

	VkImageAspectFlagBits Texture::ObtainImageAspect()
	{
		return VK_IMAGE_ASPECT_COLOR_BIT;
	}

	VkImageLayout Texture::ObtainDestLayout()
	{
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

}