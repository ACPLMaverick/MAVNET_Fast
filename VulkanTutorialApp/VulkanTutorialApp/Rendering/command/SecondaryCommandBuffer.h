#pragma once

#include "Rendering/renderPass/RenderPassCommon.h"

namespace Rendering
{
	class SecondaryCommandBuffer
	{
	public:

		typedef void* RecordContext;
		typedef void(*RecordContextFunc)(RecordContext, VkCommandBuffer);

		struct CompatibleRenderPassData
		{
			RenderPassCommon::Id Id;
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

		JE_Inline const Info* GetInfo() { return &_info; };

		JE_Inline VkCommandBuffer GetVkCommandBuffer(RenderPassCommon::Id pass, uint32_t subpass) { return _commandBuffers[GetPassIndex(pass)][subpass]; }
		JE_Inline VkCommandBuffer GetVkCommandBuffer(size_t passIndex, uint32_t subpass) { return _commandBuffers[passIndex][subpass]; }

	private:

		void CreateVkCommandBuffers();
		void RecordVkCommandBuffers();
		void DestroyVkCommandBuffers();
		void BeginRecordVkCommandBuffer(VkCommandBuffer commandBuffer, VkRenderPass renderPass, uint32_t subpass, VkFramebuffer frameBuffer);
		void EndRecordVkCommandBuffer(VkCommandBuffer commandBuffer);

		JE_Inline size_t GetPassIndex(RenderPassCommon::Id pass) 
		{ 
			return (std::find_if(_info.CompatibleRenderPasses.begin(), _info.CompatibleRenderPasses.end(), [&pass](const CompatibleRenderPassData& data) -> bool { return data.Id == pass; })) - _info.CompatibleRenderPasses.begin();
		}
		size_t GetTotalVkCommandBufferNum();


		static std::vector<VkCommandBuffer> _tmpAllocArray;

		Info _info;
		std::vector<std::vector<VkCommandBuffer>> _commandBuffers;	// For each compatible RenderPass, for each compatible subpass.
	};
}