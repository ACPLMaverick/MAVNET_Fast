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

		struct AttachmentDesc
		{
			VkFormat Format = {};	// TODO: Change some of these params to RenderTarget ptr.
			RenderState::MultisamplingMode MyMultisamplingMode = {};
			UsageMode Usage = {};
			bool bClearOnLoad = false;
			bool bStore = false;
			bool bUseStencil = false;
		};

		// TODO: Make this not vulkan-specific. Somehow.
		struct Dependency
		{
			uint32_t StageMaskSource;
			uint32_t StageMaskDest;
			uint32_t AccessMaskSource;
			uint32_t AccessMaskDest;
			uint8_t SubpassIndexSource : 4;
			uint8_t SubpassIndexDest : 4;
			uint8_t Flags : 3;

			Dependency();
		};

		static const size_t MAX_ATTACHMENTS = 7;
		static const size_t MAX_SUBPASSES = 15;

		struct Subpass
		{
			Dependency MyDependency;
			uint8_t ColorAttachmentIndices[MAX_ATTACHMENTS];
			uint8_t DepthAttachmentIndex;
			Pipeline::Type BindPoint;
			uint8_t NumColorAttachmentIndices;
		};

		struct Info
		{
			AttachmentDesc ColorAttachments[MAX_ATTACHMENTS];
			AttachmentDesc DepthStencilAttachments[MAX_ATTACHMENTS];

			Subpass Subpasses[MAX_SUBPASSES];
			RenderPassCommon::Id PassId;
			uint8_t NumColorAttachments : 4;
			uint8_t NumDepthAttachments : 4;
			uint8_t NumSubpasses;
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