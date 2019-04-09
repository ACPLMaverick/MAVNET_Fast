#include "DescriptorSet.h"

#include "Core/HelloTriangle.h"
#include "Rendering/resource/buffer/UniformBuffer.h"
#include "Rendering/resource/Texture.h"

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

	void DescriptorSet::Reinitialize()
	{
		// TODO
		JE_TODO();
	}

	void DescriptorSet::UpdateSet()
	{
		if (_bResourcesDirty)
		{
			// TODO: Make selective updating, with only things that changed updated.

			VkDescriptorBufferInfo bufferInfos[DescriptorCommon::MAX_BINDINGS_PER_LAYOUT][DescriptorCommon::MAX_DESCRIPTORS_PER_BINDING] = {};
			VkDescriptorImageInfo imageInfos[DescriptorCommon::MAX_BINDINGS_PER_LAYOUT][DescriptorCommon::MAX_DESCRIPTORS_PER_BINDING] = {};

			VkWriteDescriptorSet descriptorWrites[DescriptorCommon::MAX_BINDINGS_PER_LAYOUT] = {};

			uint32_t bufferInfosNum[DescriptorCommon::MAX_BINDINGS_PER_LAYOUT] = {};
			uint32_t imageInfosNum[DescriptorCommon::MAX_BINDINGS_PER_LAYOUT] = {};
			uint32_t descriptorWritesNum = 0;

			uint32_t bufferGlobalOffset = 0;
			uint32_t imageGlobalOffset = 0;
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
							const UniformBuffer* buffer = static_cast<const UniformBuffer*>(resource);

							VkDescriptorBufferInfo& bufferInfo = bufferInfos[i][j];
							bufferInfo.buffer = buffer->GetBuffer();
							bufferInfo.offset = 0;	// TODO..?
							bufferInfo.range = static_cast<uint32_t>(buffer->GetOptions()->DataSize);

							++bufferInfosNum[i];
						}
						else if (resource->GetType() == ResourceCommon::Type::Texture2D)
						{
							const Texture* texture = static_cast<const Texture*>(resource);

							VkDescriptorImageInfo& imageInfo = imageInfos[i][j];
							// TODO: Support other layouts.
							imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							imageInfo.imageView = texture->GetImageView();
							imageInfo.sampler = texture->GetSampler()->GetVkSampler();

							++imageInfosNum[i];
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

					VkWriteDescriptorSet& write = descriptorWrites[i];
					write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					write.dstSet = _descriptorSet;
					write.dstBinding = static_cast<uint32_t>(i);	// TODO !!!!!!!!!!!!!!!!!!!!!!!
					write.dstArrayElement = 0;

					switch (thisBindingType)
					{
					case ResourceCommon::Type::UniformBuffer:

						write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						write.descriptorCount = bufferInfosNum[i];
						write.pBufferInfo = bufferInfos[i];

						break;
					case ResourceCommon::Type::Texture2D:

						write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						write.descriptorCount = imageInfosNum[i];
						write.pImageInfo = imageInfos[i];

						break;
					default:
						JE_Assert(false);
						break;
					}

					++descriptorWritesNum;
				}
			}

			vkUpdateDescriptorSets(JE_GetRenderer()->GetDevice(), descriptorWritesNum, descriptorWrites, 0, nullptr);

			_bResourcesDirty = false;
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