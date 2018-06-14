#pragma once

namespace Rendering
{
	namespace DescriptorCommon
	{
		static const uint32_t MAX_DESCRIPTORS_PER_BINDING = 4;
		static const uint32_t MAX_BINDINGS_PER_LAYOUT = 8;

		struct LayoutInfo
		{
			struct Binding
			{
				// Stores VkShaderStageFlags
				uint8_t Stage : 8;
				uint8_t Index : 2;
				uint8_t CountMinOne : 2;
				// Stores VkDescriptorType
				uint8_t Type : 4;
				// TODO: Support immutable samplers.

				Binding()
					: Stage(0x00)
					, Index(0)
					, CountMinOne(0)
					, Type(0)
				{
				}

				JE_Inline bool IsValid() const { return Stage != 0x00; }

				JE_Inline bool operator==(const Binding& other)
				{
					return Stage == other.Stage && Index == other.Index && CountMinOne == other.CountMinOne && Type == other.Type;
				}
			};

			Binding Bindings[DescriptorCommon::MAX_BINDINGS_PER_LAYOUT];

			JE_Inline bool operator==(const LayoutInfo& other) const
			{
				return std::memcmp(this, &other, sizeof(LayoutInfo)) == 0;
			}
		};

		JE_EnumBegin(ResourceType)
			Unknown,
			Buffer,
			Image,
			TexelBuffer
		JE_EnumEnd()

		struct ResourceTypedPtr
		{
			void* ResourcePtr = nullptr;
			ResourceType ResourceT = ResourceType::Unknown;

			JE_Inline bool IsValid() const { return ResourcePtr != nullptr; }

			JE_Inline bool operator==(const ResourceTypedPtr& other) const
			{
				return ResourceT == other.ResourceT && ResourcePtr == other.ResourcePtr;
			}
		};
	}
}

namespace std
{
	template<> struct hash<::Rendering::DescriptorCommon::LayoutInfo>
	{
		size_t operator()(const ::Rendering::DescriptorCommon::LayoutInfo& layout) const
		{
			JE_AssertStatic(sizeof(::Rendering::DescriptorCommon::LayoutInfo) <= sizeof(uint64_t) * 2);
			uint64_t container0 = 0;
			uint64_t container1 = 0;
			const size_t objSizeDiv2 = sizeof(::Rendering::DescriptorCommon::LayoutInfo) / 2;
			memcpy(&container0, &layout, objSizeDiv2);
			memcpy(&container0, reinterpret_cast<const uint8_t*>(&layout) + objSizeDiv2, objSizeDiv2);
			return std::hash<uint64_t>{}(container0) ^ (std::hash<uint64_t>{}(container1) << 1);
		}
	};
}