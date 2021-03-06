#pragma once

#include "Rendering/Manager.h"
#include "DescriptorCommon.h"
#include "DescriptorSet.h"

namespace Rendering
{
	class ManagerDescriptor : public Manager
		<
			DescriptorSet::Info, 
			DescriptorSet, 
			DescriptorSet*, 
			Util::NullType, 
			std::numeric_limits<uint16_t>::max()
		>
	{
	public:
		ManagerDescriptor();
		virtual ~ManagerDescriptor();

		virtual void Cleanup() override;

		// May create new descriptor layouts.
		DescriptorCommon::LayoutData GetDescriptorLayout(const DescriptorCommon::LayoutInfo* info);

	protected:

		virtual DescriptorSet* CreateValue(const DescriptorSet::Info* key, const Util::NullType* info);
		virtual DescriptorSet* GetValueFromWrapper(DescriptorSet* const* val) override
		{
			return (*val);
		}

		VkDescriptorPool CreateDescriptorPool();
		VkDescriptorSetLayout CreateDescriptorSetLayout(const DescriptorCommon::LayoutInfo* info);
		VkDescriptorSet CreateDescriptorSet(const DescriptorCommon::LayoutData* layoutData);

	protected:

		static const uint32_t MAX_SETS_PER_POOL = 128;
		static constexpr const uint32_t _poolDescriptorCountsPerType[] = 
		{
			128,
			128,
			4
		};

		static constexpr const VkDescriptorType _poolDescriptorTypeToVk[] =
		{
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
		};

		std::vector<VkDescriptorPool> _descriptorPools;
		size_t _currentPoolIndex;

		std::vector<DescriptorSet> _descriptorSetMemory;

		std::unordered_map<DescriptorCommon::LayoutInfo, DescriptorCommon::LayoutData> _layouts;
	};
}