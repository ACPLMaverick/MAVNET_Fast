#pragma once

#include "Sampler.h"

namespace Rendering
{
	class Texture
	{
	public:

		struct LoadOptions
		{
			VkFormat DesiredFormat = VK_FORMAT_R8G8B8A8_UNORM;
			Sampler::Options SamplerOptions = Sampler::Options();
			bool bReadOnly = true;
			bool bGenerateMips = true;
		};

		struct Info
		{
			VkFormat Format;
			uint8_t* Data;
			uint32_t SizeBytes;
			uint16_t Width;
			uint16_t Height;
			uint16_t Channels;
			uint8_t MipCount;
			bool bAllocatedByStbi;

			Info()
				: Format(VK_FORMAT_UNDEFINED)
				, Data(nullptr)
				, SizeBytes(0)
				, Width(0)
				, Height(0)
				, Channels(0)
				, MipCount(1)
				, bAllocatedByStbi(false)
			{

			}
		};

	public:

		// TODO: Decide whether this returns to Renderer interface or all texture-creation-helper functions go here.
		static VkImageView UtilCreateImageView(const Texture::Info* texInfo, VkImage image, VkImageAspectFlagBits flags);

	public:

		Texture();
		~Texture();

		JE_Inline bool IsReadOnly() const { return _info.Data == nullptr; }
		JE_Inline const Info* GetInfo() const { return &_info; }
		JE_Inline VkImage GetImage() { return _image; }
		JE_Inline VkImageView GetImageView() { return _view; }
		JE_Inline const Sampler* GetSampler() const { return _sampler; }

		void Initialize(const std::string* textureName, const LoadOptions* loadOptions);
		// This takes ownership of dataToOwn data buffer and assumes it was allocated inside this app via malloc.
		void Initialize
		(
			uint8_t* dataToOwn,
			uint32_t sizeBytes,
			uint16_t width,
			uint16_t height,
			uint8_t mipCount,
			const LoadOptions* loadOptions
		);

		void Cleanup();

	protected:

		static int32_t GetDesiredChannelsFromFormat(VkFormat format);

		Texture(const Texture& copy) {}

		// This may change format if we load a compressed texture.
		void InitializeCommon(const LoadOptions* loadOptions);
		void LoadData(const std::string* loadPath, const LoadOptions* loadOptions);
		void CalculateMipCount(bool bGenerateMips);
		void CreateImage(const LoadOptions* loadOptions);
		void CreateImageView(const LoadOptions* loadOptions);
		void AssignSampler(const LoadOptions* loadOptions);
		void CleanupData();

		void GenerateMipmaps();

	protected:

		VkDeviceMemory _dataGpu;
		VkImage _image;
		VkImageView _view;

		Info _info;

		Sampler* _sampler;
	};
}