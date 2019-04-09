#pragma once

#include "Resource.h"
#include "Rendering/sampler/Sampler.h"
#include "Rendering/pipeline/RenderState.h"

namespace Rendering
{
	class Texture : public Resource
	{
	public:

		struct Info
		{
			uint8_t* Data;
			uint32_t Format;
			uint32_t SizeBytes;
			uint16_t Width;
			uint16_t Height;
			uint16_t Channels;			// TODO: Isn't this redundant to Format?
			uint8_t MipCount;
			RenderState::MultisamplingMode MyMultisamplingMode;
			bool bAllocatedByStbi;

			Info()
				: Data(nullptr)
				, Format(VK_FORMAT_UNDEFINED)
				, SizeBytes(0)
				, Width(0)
				, Height(0)
				, Channels(0)
				, MipCount(1)
				, MyMultisamplingMode(RenderState::MultisamplingMode::None)
				, bAllocatedByStbi(false)
			{

			}
		};

		struct ResizeInfo
		{
			uint16_t Width = 0;
			uint16_t Height = 0;
			bool bKeepContents = false;
		};

		struct CreateOptions
		{
			Info CreationInfo;
			Sampler::Options SamplerOptions = Sampler::Options();
			glm::vec4 ClearValuesNormalized = glm::vec4(0.0f);
			bool bClearOnCreate = true;
			bool bClearOnLoad = true;
			bool bCPUImmutable = true;
			bool bWriteOnly = false;
			bool bGenerateMips = false;
			bool bIsTransferable = false;
		};

		struct LoadOptions
		{
			VkFormat DesiredFormat = VK_FORMAT_R8G8B8A8_UNORM;
			Sampler::Options SamplerOptions = Sampler::Options();
			bool bCPUImmutable = true;
			bool bGenerateMips = true;
		};

	public:

		Texture();
		~Texture();

		JE_Inline bool IsReadOnly() const { return _info.Data == nullptr; }
		JE_Inline const Info* GetInfo() const { return &_info; }
		JE_Inline VkImage GetImage() const { return _image; }
		JE_Inline VkImageView GetImageView() const { return _view; }
		JE_Inline const Sampler* GetSampler() const { return _sampler; }

		virtual void Create(const CreateOptions* createOptions);
		void Load(const std::string& textureName, const LoadOptions* loadOptions);
		void Cleanup();

		virtual void Resize(const ResizeInfo* resizeInfo);

	protected:

		static int32_t GetDesiredChannelsFromFormat(VkFormat format);
		static void CreateImage(const ::Rendering::Texture::Info* texInfo, VkImageTiling tiling, VkImageLayout initialLayout, VkImageUsageFlags usage, VkMemoryPropertyFlags memProperties, VkImage& outImage, VkDeviceMemory& outMemory);
		static VkImageView CreateImageView(const Texture::Info* texInfo, VkImage image, VkImageAspectFlagBits flags);
		static void CopyBufferToImage(VkBuffer buffer, VkImage image, const ::Rendering::Texture::Info* texInfo);
		static void TransitionImageLayout(const ::Rendering::Texture::Info* texInfo, VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout);

		// This may change format if we load a compressed texture.
		void InitializeCommon(Sampler::Options* samplerOptions, bool bGenerateMips, bool bCPUImmutable);
		void LoadData(const std::string& loadPath, const LoadOptions* loadOptions);
		void CalculateMipCount(bool bGenerateMips);
		void CreateImage(VkImageLayout destLayout, VkImageAspectFlagBits imageAspect, VkImageUsageFlagBits imageUsage, bool bGenerateMips);
		void CreateImageView(VkImageAspectFlagBits imageAspect);
		void AssignSampler(Sampler::Options* samplerOptions);
		void CleanupData();

		void GenerateMipmaps(VkImageLayout destLayout, VkImageAspectFlagBits imageAspect);

		virtual void ClearWithFixedValue(const glm::vec4& clearValuesNormalized);
		virtual VkImageAspectFlagBits ObtainImageAspect();
		virtual VkImageLayout ObtainDestLayout();
		virtual VkImageUsageFlagBits ObtainImageUsage();
		virtual bool CanDestroyImage();

	protected:

		VkDeviceMemory _dataGpu;
		VkImage _image;
		VkImageView _view;

		Info _info;

		Sampler* _sampler;
	};
}