#pragma once

#include "DescriptorDefines.h"

namespace Rendering
{
	class ManagerDescriptor;

	class DescriptorSet
	{
		friend class ManagerDescriptor;
	public:

		struct Info
		{
			DescriptorCommon::ResourceTypedPtr Resources
				[DescriptorCommon::MAX_BINDINGS_PER_LAYOUT]
				[DescriptorCommon::MAX_DESCRIPTORS_PER_BINDING] = {};
			DescriptorCommon::LayoutInfo LayInfo = DescriptorCommon::LayoutInfo();

			JE_Inline bool CompareLayouts(const Info& other) const
			{
				return LayInfo == other.LayInfo;
			}

			JE_Inline bool operator==(const Info& other) const
			{
				return std::memcmp(this, &other, sizeof(Info)) == 0;
			}
		};

	public:

		DescriptorSet(const DescriptorSet& copy);
		~DescriptorSet();

		JE_Inline const Info* GetInfo() const { return &_info; }
		JE_Inline VkDescriptorSetLayout GetAssociatedVkDescriptorSetLayout() const { return _associatedLayout; }
		JE_Inline VkDescriptorSet GetVkDescriptorSet() const { return _descriptorSet; }

		// This object is initialized via ManagerDescriptor.

		void Cleanup();

		void UpdateSet();

		void AssignResource(const DescriptorCommon::ResourceTypedPtr resource, uint32_t binding, uint32_t slot);

	private:

		DescriptorSet();

		void UpdateCountsFromInfo();

	private:

		Info _info;
		uint32_t _bindingCount;
		uint32_t _descriptorCountPerBinding[DescriptorCommon::MAX_BINDINGS_PER_LAYOUT] = {};

		VkDescriptorSetLayout _associatedLayout;
		VkDescriptorSet _descriptorSet;

		bool _bResourcesDirty;
	};
}

namespace std
{
	template<> struct hash<::Rendering::DescriptorSet::Info>
	{
		size_t operator()(const ::Rendering::DescriptorSet::Info& layout) const
		{
			// TODO: Properly implement resource data hashing.
			return std::hash<::Rendering::DescriptorCommon::LayoutInfo>{}(layout.LayInfo);
		}
	};
}