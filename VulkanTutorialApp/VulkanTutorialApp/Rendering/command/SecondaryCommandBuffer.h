#pragma once

#include "Rendering/renderPass/RenderPassCommon.h"

namespace Rendering
{
	class RenderPass;

	class SecondaryCommandBuffer
	{
	private:

		typedef std::map<const RenderPass*, std::vector<VkCommandBuffer>> BufferMap;

	public:

		typedef void* RecordContext;
		typedef void(*RecordContextFunc)(RecordContext, VkCommandBuffer);

		struct CompatibleRenderPassData
		{
			const RenderPass* Pass;
			std::vector<uint32_t> SubpassIndices;	// Empty array will be treated as "Compatible with every subpass".
		};

		struct Info
		{
			std::vector<CompatibleRenderPassData> CompatibleRenderPasses;
			RecordContextFunc RecordFunc;
			RecordContext Context;
		};

		SecondaryCommandBuffer();
		SecondaryCommandBuffer(const SecondaryCommandBuffer& copy) = default;
		SecondaryCommandBuffer& operator=(const SecondaryCommandBuffer& copy) = default;
		~SecondaryCommandBuffer();

		void Initialize(const Info* info);
		void Cleanup();
		void Reinitialize();

		JE_Inline const Info* GetInfo() { return &_info; };

		JE_Inline VkCommandBuffer GetVkCommandBuffer(const RenderPass* pass, uint32_t subpass) 
		{
			if (pass != nullptr)
			{
				BufferMap::iterator val = _commandBuffers.find(pass);
				if (val != _commandBuffers.end())
				{
					std::vector<VkCommandBuffer>& subpassBuffers = val->second;
					if (subpass < subpassBuffers.size())
					{
						return subpassBuffers[subpass];
					}
				}
			}

			return nullptr;
		}

	private:

		static const size_t BAD_PASS_INDEX = std::numeric_limits<size_t>::max();

		void CreateVkCommandBuffers();
		void RecordVkCommandBuffers();
		void DestroyVkCommandBuffers();
		void BeginRecordVkCommandBuffer(VkCommandBuffer commandBuffer, VkRenderPass renderPass, uint32_t subpass, VkFramebuffer frameBuffer);
		void EndRecordVkCommandBuffer(VkCommandBuffer commandBuffer);

		size_t GetTotalVkCommandBufferNum();


		static std::vector<VkCommandBuffer> _tmpAllocArray;

		Info _info;
		BufferMap _commandBuffers;	// For each compatible RenderPass, for each compatible subpass.
	};
}