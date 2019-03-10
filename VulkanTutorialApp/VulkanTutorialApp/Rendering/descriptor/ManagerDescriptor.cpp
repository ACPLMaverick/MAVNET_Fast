#include "ManagerDescriptor.h"

#include "Core/HelloTriangle.h"

#include "Rendering/resource/Resource.h"

namespace Rendering
{
	ManagerDescriptor::ManagerDescriptor()
		: Manager()
		, _currentPoolIndex(-1)
	{
	}

	ManagerDescriptor::~ManagerDescriptor()
	{
		JE_Assert(_descriptorSetMemory.size() == 0);
		JE_Assert(_descriptorPools.size() == 0);
		JE_Assert(_layouts.size() == 0);
		JE_Assert(_currentPoolIndex == -1);
	}

	void ManagerDescriptor::Cleanup()
	{
		for (auto& pool : _descriptorPools)
		{
			vkDestroyDescriptorPool(JE_GetRenderer()->GetDevice(), pool, JE_GetRenderer()->GetAllocatorPtr());
		}
		_descriptorPools.clear();
		_currentPoolIndex = -1;

		for (auto& layoutData : _layouts)
		{
			vkDestroyDescriptorSetLayout(JE_GetRenderer()->GetDevice(), layoutData.second.Layout, JE_GetRenderer()->GetAllocatorPtr());
		}
		_layouts.clear();

		Manager::Cleanup();
	}

	DescriptorSet * ManagerDescriptor::CreateValue(const DescriptorSet::Info * key, const Util::NullType * info)
	{
		// Descriptor set not found! Get layout for this descriptor set then.
		DescriptorCommon::LayoutData layout = GetDescriptorLayout(&key->LayInfo);

		// Create new descriptor set with these parameters.
		DescriptorSet* set = AllocateValue();

		set->_info = *key;
		set->_associatedLayout = layout;
		set->_descriptorSet = CreateDescriptorSet(&layout);

		set->_bResourcesDirty = true;
		set->UpdateSet();

		return set;
	}

	DescriptorCommon::LayoutData ManagerDescriptor::GetDescriptorLayout(const DescriptorCommon::LayoutInfo * info)
	{
		auto it = _layouts.find(*info);
		if (it != _layouts.end())
		{
			return it->second;
		}
		else
		{
			// Descriptor layout not found! Allocate new one.
			DescriptorCommon::LayoutData retData;

			retData.Layout = CreateDescriptorSetLayout(info);
			retData.Info = *info;
			_layouts.emplace(*info, retData);

			return retData;
		}
	}

	VkDescriptorPool ManagerDescriptor::CreateDescriptorPool()
	{
		VkDescriptorPool pool;

		VkDescriptorPoolSize sizeInfoArray[ResourceCommon::Type_ShaderResourceNum];

		JE_AssertStatic(JE_ArrayLength(_poolDescriptorCountsPerType) >= ResourceCommon::Type_ShaderResourceNum);
		JE_AssertStatic(JE_ArrayLength(_poolDescriptorTypeToVk) >= ResourceCommon::Type_ShaderResourceNum);

		for (size_t i = 0; i < ResourceCommon::Type_ShaderResourceNum; ++i)
		{
			sizeInfoArray[i].descriptorCount = _poolDescriptorCountsPerType[i];
			sizeInfoArray[i].type = _poolDescriptorTypeToVk[i];
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(ResourceCommon::Type_ShaderResourceNum);
		poolInfo.pPoolSizes = sizeInfoArray; // Omit Unknown.
		poolInfo.maxSets = MAX_SETS_PER_POOL;

		/*

		// Legacy code

		std::array<VkDescriptorPoolSize, 2> poolSizes = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 1;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = 1;

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1;
		*/

		JE_AssertThrowVkResult(vkCreateDescriptorPool(JE_GetRenderer()->GetDevice(), &poolInfo, JE_GetRenderer()->GetAllocatorPtr(), &pool));

		return pool;
	}

	VkDescriptorSetLayout ManagerDescriptor::CreateDescriptorSetLayout(const DescriptorCommon::LayoutInfo * info)
	{
		VkDescriptorSetLayout layout;

		VkDescriptorSetLayoutBinding bindingArray[DescriptorCommon::MAX_BINDINGS_PER_LAYOUT] = {};

		uint32_t validBindings = 0;
		for (size_t i = 0; i < DescriptorCommon::MAX_BINDINGS_PER_LAYOUT; ++i)
		{
			const DescriptorCommon::LayoutInfo::Binding& binding = info->Bindings[i];

			if(!binding.IsValid())
				break;	// Do not go further if encounter invalid binding. Bindings should be tightly packed.

			++validBindings;

			bindingArray[i].binding = binding.Index;
			bindingArray[i].descriptorType = static_cast<VkDescriptorType>(binding.Type);
			bindingArray[i].descriptorCount = binding.Count;
			bindingArray[i].stageFlags = static_cast<VkShaderStageFlags>(binding.Stage);
			bindingArray[i].pImmutableSamplers = nullptr; // TODO: For textures probably.

			JE_Assert(bindingArray[i].descriptorCount <= DescriptorCommon::MAX_DESCRIPTORS_PER_BINDING);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = validBindings;
		layoutInfo.pBindings = bindingArray;

		/*

		// Legacy code

		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		*/

		JE_AssertThrowVkResult(vkCreateDescriptorSetLayout(JE_GetRenderer()->GetDevice(), &layoutInfo, JE_GetRenderer()->GetAllocatorPtr(), &layout));

		return layout;
	}

	VkDescriptorSet ManagerDescriptor::CreateDescriptorSet(const DescriptorCommon::LayoutData * layoutData)
	{
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layoutData->Layout;

		if (_currentPoolIndex != -1)
		{
			allocInfo.descriptorPool = _descriptorPools[_currentPoolIndex];
			VkResult res = vkAllocateDescriptorSets(JE_GetRenderer()->GetDevice(), &allocInfo, &descriptorSet);
			JE_Assert(res != VK_ERROR_OUT_OF_HOST_MEMORY && res != VK_ERROR_OUT_OF_DEVICE_MEMORY);
			if (res == VK_SUCCESS)
			{
				return descriptorSet;
			}
		}

		// Unable to allocate from current pool, so try the next one.
		// Assume that nothing will be deallocated from the pools for now. TODO.

		VkDescriptorPool newPool = CreateDescriptorPool();
		_descriptorPools.push_back(newPool);
		_currentPoolIndex = _descriptorPools.size() - 1;

		allocInfo.descriptorPool = newPool;
		JE_AssertThrowVkResult(vkAllocateDescriptorSets(JE_GetRenderer()->GetDevice(), &allocInfo, &descriptorSet));

		return descriptorSet;
	}


	constexpr const uint32_t ManagerDescriptor::_poolDescriptorCountsPerType[];
	constexpr const VkDescriptorType ManagerDescriptor::_poolDescriptorTypeToVk[];
}