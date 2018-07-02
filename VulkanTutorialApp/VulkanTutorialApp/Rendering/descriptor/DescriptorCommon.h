#pragma once

#include "Core/GlobalIncludes.h"

#include "Rendering/resource/ResourceCommon.h"

namespace Rendering
{
	namespace DescriptorCommon
	{
		static const uint32_t MAX_BINDINGS_PER_LAYOUT = 8;
		static const uint32_t MAX_DESCRIPTORS_PER_BINDING = 15;

		JE_EnumBegin(ShaderStage)

			Vertex = VK_SHADER_STAGE_VERTEX_BIT,
			TesselationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
			TesselationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
			Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
			Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
			Compute = VK_SHADER_STAGE_COMPUTE_BIT,
			AllGraphics = VK_SHADER_STAGE_ALL_GRAPHICS

		JE_EnumEnd()

		extern VkShaderStageFlagBits ShaderStageToVkShaderStageFlag(ShaderStage stage);

		struct LayoutInfo
		{
			struct Binding
			{
				// Stores VkShaderStageFlags (5 bits is enough)
				uint8_t Stage : 6;
				uint8_t Index : 2;
				uint8_t Count : 4;
				// Stores VkDescriptorType
				uint8_t Type : 4;
				// TODO: Support immutable samplers.

				Binding()
					: Stage(0x00)
					, Index(0)
					, Count(0)
					, Type(0)
				{
				}

				Binding(const Binding& copy)
					: Stage(copy.Stage)
					, Index(copy.Index)
					, Count(copy.Count)
					, Type(copy.Type)
				{
				}

				Binding(
					ShaderStage stage
					, uint8_t index
					, uint8_t count
					, ResourceCommon::Type type)
					: Stage(static_cast<uint8_t>(ShaderStageToVkShaderStageFlag(stage)))
					, Index(index)
					, Count(count)
					, Type(static_cast<uint8_t>(ResourceCommon::TypeToDescriptorType(type)))
				{
				}

				JE_Inline bool IsValid() const { return Stage != 0x00; }

				JE_Inline Binding& operator=(const Binding& copy)
				{
					Stage = copy.Stage;
					Index = copy.Index;
					Count = copy.Count;
					Type = copy.Type;

					return *this;
				}

				JE_Inline bool operator==(const Binding& other)
				{
					return Stage == other.Stage && Index == other.Index && Count == other.Count && Type == other.Type;
				}
			};

			Binding Bindings[DescriptorCommon::MAX_BINDINGS_PER_LAYOUT];

			JE_Inline bool operator==(const LayoutInfo& other) const
			{
				return std::memcmp(this, &other, sizeof(LayoutInfo)) == 0;
			}
		};

		struct LayoutData
		{
			DescriptorCommon::LayoutInfo Info = DescriptorCommon::LayoutInfo();
			VkDescriptorSetLayout Layout = VK_NULL_HANDLE;
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