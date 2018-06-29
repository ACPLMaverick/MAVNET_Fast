#include "DescriptorSet.h"

#include "Core/HelloTriangle.h"
#include "UniformBuffer.h"
#include "Texture.h"

namespace Rendering
{
	DescriptorSet::DescriptorSet()
		: _info(Info())
		, _bindingCount(0)
		, _associatedLayout(DescriptorCommon::LayoutData())
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
		JE_Assert(_associatedLayout.Layout == VK_NULL_HANDLE);
		JE_Assert(_descriptorSet == VK_NULL_HANDLE);
	}

	void DescriptorSet::Cleanup()
	{
		_info = Info();
		_bindingCount = 0;
		_associatedLayout.Layout = VK_NULL_HANDLE;
		_descriptorSet = VK_NULL_HANDLE;
		_bResourcesDirty = false;
	}

	void DescriptorSet::UpdateSet()
	{
		if (_bResourcesDirty)
		{
			// TODO: Make selective updating, with only things that changed updated.

			std::vector<VkDescriptorBufferInfo> bufferInfos;
			std::vector<VkDescriptorImageInfo> imageInfos;

			std::vector<VkWriteDescriptorSet> descriptorWrites;

			size_t bufferGlobalOffset = 0;
			size_t imageGlobalOffset = 0;
			for (size_t i = 0; i < DescriptorCommon::MAX_BINDINGS_PER_LAYOUT; ++i)
			{
				ResourceCommon::Type thisBindingType = ResourceCommon::Type::Unknown;
				for (size_t j = 0; j < DescriptorCommon::MAX_DESCRIPTORS_PER_BINDING; ++j)
				{
					const Resource* resource = _info.Resources[i][j];
					if (resource != nullptr)
					{
						if (thisBindingType == ResourceCommon::Type::Unknown)
						{
							thisBindingType = resource->GetType();
						}
						else if (
							thisBindingType != ResourceCommon::Type::Unknown
							&& thisBindingType != resource->GetType())
						{
							JE_Assert(false, "Assigning different resource types to the same binding!");
						}

						if (resource->GetType() == ResourceCommon::Type::UniformBuffer)
						{
							const UniformBuffer* buffer = reinterpret_cast<const UniformBuffer*>(resource);

							VkDescriptorBufferInfo bufferInfo = {};
							bufferInfo.buffer = buffer->GetBuffer();
							bufferInfo.offset = 0;
							bufferInfo.range = static_cast<uint32_t>(buffer->GetOptions()->DataSize);

							bufferInfos.push_back(bufferInfo);
						}
						else if (resource->GetType() == ResourceCommon::Type::Texture2D)
						{
							const Texture* texture = reinterpret_cast<const Texture*>(resource);

							VkDescriptorImageInfo imageInfo = {};
							// TODO: Support other layouts.
							imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							imageInfo.imageView = texture->GetImageView();
							imageInfo.sampler = texture->GetSampler()->GetVkSampler();

							imageInfos.push_back(imageInfo);
						}
						else
						{
							JE_Assert(false, "Resource is valid but have an unsupported type!");
						}
					}
					else
					{
						// "Holes" in resource layout are not supported. Please avoid them.
						break;
					}
				}

				if (thisBindingType != ResourceCommon::Type::Unknown)
				{
					// Check if this/these resource[s] corresponds with layout.
					JE_Assert(
						ResourceCommon::TypeToDescriptorType(thisBindingType) == static_cast<VkDescriptorType>(_info.LayInfo.Bindings[i].Type), "Assigned resource does not match the layout.");

					VkWriteDescriptorSet write = {};
					write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					write.dstSet = _descriptorSet;
					write.dstBinding = static_cast<uint32_t>(i);
					write.dstArrayElement = 0;

					switch (thisBindingType)
					{
					case ResourceCommon::Type::UniformBuffer:

						write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						write.descriptorCount = static_cast<uint32_t>(bufferInfos.size());
						write.pBufferInfo = bufferInfos.data();

						break;
					case ResourceCommon::Type::Texture2D:

						write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						write.descriptorCount = static_cast<uint32_t>(imageInfos.size());
						write.pImageInfo = imageInfos.data();

						break;
					default:
						JE_Assert(false);
						break;
					}

					descriptorWrites.push_back(write);

					imageInfos.clear();
					bufferInfos.clear();
				}
			}

			vkUpdateDescriptorSets(JE_GetRenderer()->GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

			_bResourcesDirty = false;

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
		}
	}

	void DescriptorSet::AssignResource(const Resource* resource, uint32_t binding, uint32_t slot)
	{
		JE_Assert(resource != nullptr);
		JE_Assert(binding < DescriptorCommon::MAX_BINDINGS_PER_LAYOUT);
		JE_Assert(slot < DescriptorCommon::MAX_DESCRIPTORS_PER_BINDING);

		JE_Assert(static_cast<VkDescriptorType>(_info.LayInfo.Bindings[binding].Type) == ResourceCommon::TypeToDescriptorType(resource->GetType()));

		if (_info.Resources[binding][slot] != nullptr)
		{
			JE_PrintWarnLine("Assigning to an occupied resource slot!");
		}

		_info.Resources[binding][slot] = resource;

		_bResourcesDirty = true;
	}

	bool DescriptorSet::TryAssignResource(const Resource * resource)
	{
		uint32_t binding, slot;
		if (GetFirstAvailableResourceSlot(resource, &binding, &slot))
		{
			AssignResource(resource, binding, slot);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool DescriptorSet::TryAssignResource(const Resource * resource, uint32_t * outBinding, uint32_t * outSlot)
	{
		if (GetFirstAvailableResourceSlot(resource, outBinding, outSlot))
		{
			AssignResource(resource, *outBinding, *outSlot);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool DescriptorSet::GetFirstAvailableResourceSlot(const Resource * resource, uint32_t * outBinding, uint32_t * outSlot)
	{
		if (resource == nullptr || resource->GetType() == ResourceCommon::Type::Unknown)
		{
			return false;
		}

		for (uint32_t i = 0; i < DescriptorCommon::MAX_BINDINGS_PER_LAYOUT; ++i)
		{
			if (static_cast<VkDescriptorType>(_info.LayInfo.Bindings[i].Type) == ResourceCommon::TypeToDescriptorType(resource->GetType()))
			{
				for (uint32_t j = 0; j < DescriptorCommon::MAX_DESCRIPTORS_PER_BINDING; ++j)
				{
					if (_info.Resources[i][j] == nullptr)
					{
						*outBinding = i;
						*outSlot = j;
						return true;
					}
				}
			}
		}

		return false;
	}

	void DescriptorSet::FillPipelineKey(PipelineKey * key) const
	{
	}
}