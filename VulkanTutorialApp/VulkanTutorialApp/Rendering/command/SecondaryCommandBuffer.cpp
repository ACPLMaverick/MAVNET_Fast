#include "SecondaryCommandBuffer.h"

#include "Core/HelloTriangle.h"
#include "Rendering/renderPass/ManagerRenderPass.h"

namespace Rendering
{
	SecondaryCommandBuffer::SecondaryCommandBuffer()
		: _info()
	{
	}

	SecondaryCommandBuffer::~SecondaryCommandBuffer()
	{
	}

	void SecondaryCommandBuffer::Initialize(const Info * info)
	{
		JE_Assert(info);
		JE_Assert(info->RecordFunc);
		_info = *info;
		
		CreateVkCommandBuffers();
		RecordVkCommandBuffers();
	}

	void SecondaryCommandBuffer::Cleanup()
	{
		DestroyVkCommandBuffers();
		_commandBuffers.clear();
	}

	void SecondaryCommandBuffer::Reinitialize()
	{
		JE_Assert(_info.RecordFunc);
		Cleanup();
		CreateVkCommandBuffers();
		RecordVkCommandBuffers();
	}

	void SecondaryCommandBuffer::CreateVkCommandBuffers()
	{
		const size_t totalNum = GetTotalVkCommandBufferNum();
		if (_tmpAllocArray.size() < totalNum)
		{
			_tmpAllocArray.resize(totalNum);
		}

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocInfo.commandPool = JE_GetRenderer()->GetCommandPoolStatic();
		allocInfo.commandBufferCount = (uint32_t)totalNum;

		JE_AssertVkResult(vkAllocateCommandBuffers(JE_GetRenderer()->GetDevice(), &allocInfo, _tmpAllocArray.data()));

		size_t tmpBufferIndex = 0;
		for (CompatibleRenderPassData& subpassData : _info.CompatibleRenderPasses)
		{
			_commandBuffers.insert(std::make_pair(subpassData.Pass, std::vector<VkCommandBuffer>()));
			std::vector<VkCommandBuffer>& currentArray = _commandBuffers[subpassData.Pass];

			size_t subpassNum = subpassData.SubpassIndices.size();
			if (subpassNum == 0)
			{
				const RenderPass* pass = subpassData.Pass;
				JE_Assert(pass);

				subpassNum = pass->GetInfo()->NumSubpasses;

				for (size_t i = 0; i < subpassNum; ++i)
				{
					subpassData.SubpassIndices.push_back((uint32_t)i);
				}
			}

			for (size_t i = 0; i < subpassNum; ++i)
			{
				currentArray.push_back(_tmpAllocArray[tmpBufferIndex]);
				++tmpBufferIndex;
			}
		}
		JE_Assert(tmpBufferIndex == totalNum);
	}

	void SecondaryCommandBuffer::RecordVkCommandBuffers()
	{
		const size_t compatibleRenderPassesSize = _info.CompatibleRenderPasses.size();
		for (size_t i = 0; i < compatibleRenderPassesSize; ++i)
		{
			const RenderPass* pass = _info.CompatibleRenderPasses[i].Pass;
			JE_Assert(pass);

			std::vector<VkCommandBuffer>& bufferArray = _commandBuffers[pass];
			const size_t bufferNum = bufferArray.size();

			for (size_t j = 0; j < bufferNum; ++j)
			{
				VkCommandBuffer buffer = bufferArray[j];

				BeginRecordVkCommandBuffer(buffer, pass->GetVkRenderPass(), _info.CompatibleRenderPasses[i].SubpassIndices[j], VK_NULL_HANDLE);	// TODO: Acquire proper framebuffer, for now it is not necessary.
				_info.RecordFunc(_info.Context, buffer);
				EndRecordVkCommandBuffer(buffer);
			}
		}
	}

	void SecondaryCommandBuffer::DestroyVkCommandBuffers()
	{
		// There has to be enough places in static alloc array, because it would have been enlarged on creation of this buffer.

		size_t allocArrayIndex = 0;
		for (BufferMap::iterator it = _commandBuffers.begin(); it != _commandBuffers.end(); ++it)
		{
			std::vector<VkCommandBuffer>& bufferArray = it->second;
			const size_t bufferNum = bufferArray.size();

			for (size_t j = 0; j < bufferNum; ++j)
			{
				VkCommandBuffer buffer = bufferArray[j];
				_tmpAllocArray[allocArrayIndex] = buffer;
				++allocArrayIndex;
			}
		}

		vkFreeCommandBuffers(JE_GetRenderer()->GetDevice(), JE_GetRenderer()->GetCommandPoolStatic(), (uint32_t)allocArrayIndex, _tmpAllocArray.data());
	}

	void SecondaryCommandBuffer::BeginRecordVkCommandBuffer(VkCommandBuffer commandBuffer, VkRenderPass renderPass, uint32_t subpass, VkFramebuffer frameBuffer)
	{
		VkCommandBufferInheritanceInfo inheritanceInfo = {};
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.renderPass = renderPass;
		inheritanceInfo.subpass = subpass;
		inheritanceInfo.framebuffer = frameBuffer;

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // It has to be simultaneous for Tutorial/Triple buffering.
		beginInfo.pInheritanceInfo = &inheritanceInfo;
		JE_AssertVkResult(vkBeginCommandBuffer(commandBuffer, &beginInfo));
	}

	void SecondaryCommandBuffer::EndRecordVkCommandBuffer(VkCommandBuffer commandBuffer)
	{
		JE_AssertVkResult(vkEndCommandBuffer(commandBuffer));
	}

	size_t SecondaryCommandBuffer::GetTotalVkCommandBufferNum()
	{
		size_t counter = 0;

		for (const CompatibleRenderPassData& subpassData : _info.CompatibleRenderPasses)
		{
			if (subpassData.SubpassIndices.empty())
			{
				const RenderPass* pass = subpassData.Pass;
				JE_Assert(pass);

				counter += pass->GetInfo()->NumSubpasses;
			}
			else
			{
				counter += subpassData.SubpassIndices.size();
			}
		}

		return counter;
	}


	std::vector<VkCommandBuffer> SecondaryCommandBuffer::_tmpAllocArray(64);
}