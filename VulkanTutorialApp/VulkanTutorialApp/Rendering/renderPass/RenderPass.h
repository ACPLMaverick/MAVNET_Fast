#pragma once

#include "Rendering/pipeline/RenderState.h"
#include "RenderPassCommon.h"
#include "Rendering/pipeline/Pipeline.h"

namespace Rendering
{
	class RenderPass
	{
	public:

		JE_EnumBegin(UsageMode)
			Color
			, ColorPresentable
			, Transferable
			, DepthStencil
		JE_EnumEnd()

		struct Attachment
		{
			VkFormat Format;	// TODO: Change some of these params to RenderTarget ptr.
			RenderState::MultisamplingMode MyMultisamplingMode;
			UsageMode Usage;
			bool bClearOnLoad;
			bool bStore;
			bool bUseStencil;
		};

		// TODO: Make this not vulkan-specific. Somehow.
		struct Dependency
		{
			uint32_t SubpassIndexSource;
			uint32_t SubpassIndexDest;
			uint32_t StageMaskSource;
			uint32_t StageMaskDest;
			uint32_t AccessMaskSource;
			uint32_t AccessMaskDest;
			uint32_t Flags;
		};

		struct Subpass
		{
			std::vector<uint32_t> ColorAttachmentIndices;
			uint32_t DepthAttachmentIndex;
			Pipeline::Type BindPoint;
		};

		struct Info
		{
			std::vector<Attachment> ColorAttachments;
			std::vector<Attachment> DepthStencilAttachments;

			std::vector<Subpass> Subpasses;
			std::vector<Dependency> Dependencies;
			RenderPassCommon::Id PassId;
		};

	public:

		RenderPass() { }
		~RenderPass();

		void Initialize(const Info* info);
		void Cleanup();
		void Reinitialize();

		JE_Inline const Info* GetInfo() const { return &_info; }
		JE_Inline VkRenderPass GetVkRenderPass() const { return _renderPass; }

	private:

		Info _info;

		VkRenderPass _renderPass = VK_NULL_HANDLE;
	};
}