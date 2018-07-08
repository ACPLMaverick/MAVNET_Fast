#pragma once

#include "Rendering/Manager.h"
#include "DescriptorCommon.h"
#include "DescriptorSet.h"

namespace Rendering
{
	class ManagerDescriptor : public Manager<DescriptorSet::Info, DescriptorSet, DescriptorSet*>
	{
	public:
		ManagerDescriptor();
		virtual ~ManagerDescriptor();

		virtual void Cleanup() override;

	protected:

		virtual DescriptorSet* CreateValue(const DescriptorSet::Info* key, const Util::NullType* info) override;
		virtual bool IsValidValueWrapper(DescriptorSet* const* val) override
		{
			return (*val) != nullptr;
		}

		// May create new descriptor layouts.
		DescriptorCommon::LayoutData GetDescriptorLayout(const DescriptorCommon::LayoutInfo* info);

		VkDescriptorPool CreateDescriptorPool();
		VkDescriptorSetLayout CreateDescriptorSetLayout(const DescriptorCommon::LayoutInfo* info);
		VkDescriptorSet CreateDescriptorSet(const DescriptorCommon::LayoutData* layoutData);

	protected:

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

		std::unordered_map<DescriptorCommon::LayoutInfo, DescriptorCommon::LayoutData> _layouts;
	};
}