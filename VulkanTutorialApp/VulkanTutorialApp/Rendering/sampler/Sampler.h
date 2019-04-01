#pragma once

namespace Rendering
{
	class Sampler
	{
	public:

		JE_EnumBegin(BorderColor)

			OpaqueBlack,
			OpaqueWhite

		JE_EnumEnd()


		JE_EnumBegin(AddressMode)

			Clamp,
			Wrap,
			Mirror,
			Border

		JE_EnumEnd()


		JE_EnumBegin(FilterMode)

			Linear,
			Point

		JE_EnumEnd()


		struct Options
		{
			uint8_t OptAnisotropyLevel = 16;
			// Zero indicates that mipmap level will be overwritten by texture's if assigning sampler via texture initialization. Otherwise, it is illegal to use this option directly.
			uint8_t OptMaxMipmapLevel = 0;
			BorderColor OptBorderColor = BorderColor::OpaqueBlack;
			AddressMode OptAddressMode = AddressMode::Clamp;
			FilterMode OptFilterMode = FilterMode::Linear;
			FilterMode OptMipmapFilterMode = FilterMode::Linear;

			bool operator==(const Options& other) const
			{
				JE_AssertStatic(sizeof(Options) <= sizeof(uint64_t));

				uint64_t one = 0;
				uint64_t two = 0;

				memcpy(&one, this, sizeof(Options));
				memcpy(&two, &other, sizeof(Options));

				return one == two;
			}
		};

	public:

		JE_Inline static VkBorderColor UtilBorderColorToVk(BorderColor opt) { return _borderColorToVk[static_cast<uint8_t>(opt)]; }
		JE_Inline static VkSamplerAddressMode UtilAddressModeToVk(AddressMode opt) { return _addressModeToVk[static_cast<uint8_t>(opt)]; }
		JE_Inline static VkFilter UtilFilterModeToVkFilter(FilterMode opt) { return _filterModeToVkFilter[static_cast<uint8_t>(opt)]; }
		JE_Inline static VkSamplerMipmapMode UtilFilterModeToVkMipmapMode(FilterMode opt) { return _filterModeToVkMipmapMode[static_cast<uint8_t>(opt)]; }

	public:

		Sampler();
		Sampler(const Sampler& copy);
		~Sampler();

		JE_Inline bool IsInitialized() const { return _sampler != VK_NULL_HANDLE; }
		JE_Inline VkSampler GetVkSampler() const { return _sampler; }
		JE_Inline const Options* GetOptions() const { return &_options; }

		void Initialize(const Options* options);
		void Cleanup();
		void Reinitialize();

	protected:

		static constexpr const VkBorderColor _borderColorToVk[] =
		{
			VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
		};

		static constexpr const VkSamplerAddressMode _addressModeToVk[] = 
		{
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
		};

		static constexpr const VkFilter _filterModeToVkFilter[] =
		{
			VK_FILTER_LINEAR,
			VK_FILTER_NEAREST
		};

		static constexpr const VkSamplerMipmapMode _filterModeToVkMipmapMode[] =
		{
			VK_SAMPLER_MIPMAP_MODE_LINEAR,
			VK_SAMPLER_MIPMAP_MODE_NEAREST
		};


	protected:

		VkSampler _sampler;
		Options _options;

	protected:

		void CreateSampler(const Options* options);
	};
}

namespace std
{
	template<> struct hash<Rendering::Sampler::Options>
	{
		size_t operator()(const Rendering::Sampler::Options& opts) const
		{
			uint64_t container = 0;
			memcpy(&container, &opts, sizeof(Rendering::Sampler::Options));
			return std::hash<uint64_t>{}(container);
		}
	};
}