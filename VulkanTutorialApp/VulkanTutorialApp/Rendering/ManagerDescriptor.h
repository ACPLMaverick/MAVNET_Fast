#pragma once

#include "DescriptorDefines.h"
#include "DescriptorSet.h"

namespace Rendering
{
	class ManagerDescriptor
	{
	public:

	public:
		ManagerDescriptor();
		~ManagerDescriptor();

		void Initialize();
		void Cleanup();

		// May return nullptr if such descriptor set does not exist.
		DescriptorSet* TryGetDescriptorSet(const DescriptorSet::Info* info);
		// May allocate new descriptor sets.
		DescriptorSet* GetDescriptorSet(const DescriptorSet::Info* info);

	private:

		struct LayoutData
		{
			DescriptorCommon::LayoutInfo Info = DescriptorCommon::LayoutInfo();
			VkDescriptorSetLayout Layout = VK_NULL_HANDLE;
		};

	private:

		// May create new descriptor layouts.
		LayoutData GetDescriptorLayout(const DescriptorCommon::LayoutInfo* info);

		VkDescriptorPool CreateDescriptorPool();
		VkDescriptorSetLayout CreateDescriptorSetLayout(const DescriptorCommon::LayoutInfo* info);
		VkDescriptorSet CreateDescriptorSet(const LayoutData* layoutData);

		void AllocateDescriptorSet(const LayoutData* layoutData, VkDescriptorSet descriptorSet);

	private:

		static const uint32_t MAX_SETS_PER_POOL = 100;
		static constexpr const uint32_t _poolDescriptorCountsPerType[] = 
		{
			0,
			128,
			128,
			4
		};

		static constexpr const VkDescriptorType _poolDescriptorTypeToVk[] =
		{
			static_cast<VkDescriptorType>(-1),
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
		};

		std::vector<VkDescriptorPool> _descriptorPools;
		size_t _currentPoolIndex;

		std::vector<DescriptorSet> _descriptorSetMemory;

		std::unordered_map<DescriptorCommon::LayoutInfo, LayoutData> _layouts;
		std::unordered_map<DescriptorSet::Info, DescriptorSet*> _sets;
	};
}