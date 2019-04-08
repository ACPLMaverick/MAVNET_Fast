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
			uint16_t Channels;
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
			uint16_t Width;
			uint16_t Height;
		};

		struct CreateOptions
		{
			Info CreationInfo;
			Sampler::Options SamplerOptions = Sampler::Options();
			glm::vec4 ClearValuesNormalized = glm::vec4(0.0f);
			bool bClearOnCreate = true;
			bool bClearOnLoad = true;
			bool bReadOnly = true;
			bool bGenerateMips = true;
			bool bIsTransferable = false;
		};

		struct LoadOptions
		{
			VkFormat DesiredFormat = VK_FORMAT_R8G8B8A8_UNORM;
			Sampler::Options SamplerOptions = Sampler::Options();
			bool bReadOnly = true;
			bool bGenerateMips = true;
		};

	public:

		// TODO: Decide whether this returns to Renderer interface or all texture-creation-helper functions go here.
		static VkImageView UtilCreateImageView(const Texture::Info* texInfo, VkImage image, VkImageAspectFlagBits flags);

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

		// This may change format if we load a compressed texture.
		void InitializeCommon(Sampler::Options* samplerOptions, bool bGenerateMips, bool bReadOnly);
		void LoadData(const std::string& loadPath, const LoadOptions* loadOptions);
		void CalculateMipCount(bool bGenerateMips);
		void CreateImage(VkImageLayout destLayout, VkImageAspectFlagBits imageAspect, bool bGenerateMips);
		void CreateImageView(VkImageAspectFlagBits imageAspect);
		void AssignSampler(Sampler::Options* samplerOptions);
		void CleanupData();

		void GenerateMipmaps(VkImageLayout destLayout, VkImageAspectFlagBits imageAspect);

		virtual void ClearWithFixedValue(const glm::vec4& clearValuesNormalized);
		virtual VkImageAspectFlagBits ObtainImageAspect();
		virtual VkImageLayout ObtainDestLayout();

	protected:

		VkDeviceMemory _dataGpu;
		VkImage _image;
		VkImageView _view;

		Info _info;

		Sampler* _sampler;
	};
}