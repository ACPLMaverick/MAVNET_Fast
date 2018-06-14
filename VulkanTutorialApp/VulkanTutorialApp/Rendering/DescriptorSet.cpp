#include "DescriptorSet.h"


namespace Rendering
{
	DescriptorSet::DescriptorSet()
		: _info(Info())
		, _bindingCount(0)
		, _associatedLayout(VK_NULL_HANDLE)
		, _descriptorSet(VK_NULL_HANDLE)
		, _bResourcesDirty(false)
	{
		memset(_descriptorCountPerBinding, 0, sizeof(_descriptorCountPerBinding));
	}

	DescriptorSet::DescriptorSet(const DescriptorSet & copy)
		: _info(copy._info)
		, _bindingCount(copy._bindingCount)
		, _associatedLayout(copy._associatedLayout)
		, _descriptorSet(copy._descriptorSet)
		, _bResourcesDirty(copy._bResourcesDirty)
	{
		memcpy(_descriptorCountPerBinding, copy._descriptorCountPerBinding, sizeof(_descriptorCountPerBinding));
	}

	DescriptorSet::~DescriptorSet()
	{
		JE_Assert(_associatedLayout == VK_NULL_HANDLE);
		JE_Assert(_descriptorSet == VK_NULL_HANDLE);
	}

	void DescriptorSet::Cleanup()
	{
		_info = Info();
		_bindingCount = 0;
		_associatedLayout = VK_NULL_HANDLE;
		_descriptorSet = VK_NULL_HANDLE;
		_bResourcesDirty = false;
	}

	void DescriptorSet::UpdateSet()
	{
		if (_bResourcesDirty)
		{
			/*

			// Legacy code.

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = _uniformBuffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject); // If you're overwriting the whole buffer, like we are in this case, then it is is also possible to use the VK_WHOLE_SIZE value for the range. 

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = _texture.GetImageView();
			imageInfo.sampler = _texture.GetSampler()->GetVkSampler();

			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = _descriptorSet;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;
			descriptorWrites[0].pTexelBufferView = nullptr;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = _descriptorSet;
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = nullptr;
			descriptorWrites[1].pImageInfo = &imageInfo;
			descriptorWrites[1].pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			*/

			_bResourcesDirty = false;
		}
	}

	void DescriptorSet::AssignResource(const DescriptorCommon::ResourceTypedPtr resource, uint32_t binding, uint32_t slot)
	{
	}

	void DescriptorSet::UpdateCountsFromInfo()
	{
	}
}