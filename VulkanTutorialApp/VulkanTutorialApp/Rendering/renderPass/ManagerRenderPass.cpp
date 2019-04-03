#include "ManagerRenderPass.h"

#include "Core/HelloTriangle.h"

namespace Rendering
{
	void ManagerRenderPass::Initialize()
	{
		Manager::Initialize();

		// Create all RenderPasses here, for each RenderPass:Id.

		for (uint64_t i = 0; i < static_cast<uint64_t>(RenderPassCommon::Id::ENUM_SIZE); ++i)
		{
			if(static_cast<RenderPassCommon::Id>(i) == RenderPassCommon::Id::Custom)
				continue; // TODO:Implement.

			RenderPass* pass = AllocateValue();
			RenderPass::Info info;

			FillUpInfoForRenderPassId(static_cast<RenderPassCommon::Id>(i), &info);

			pass->Initialize(&info);
			_map.emplace(i, pass);
		}
	}

	RenderPass * ManagerRenderPass::CreateValue(const RenderPassKey * key, const RenderPass::Info * initData)
	{
		// TODO: Implement.
		JE_Assert(false);
		return nullptr;
	}

	void ManagerRenderPass::FillUpInfoForRenderPassId(RenderPassCommon::Id id, RenderPass::Info * outInfo)
	{
		outInfo->PassId = id;

		switch (id)
		{
		case Rendering::RenderPassCommon::Id::Tutorial:
		{
			RenderPass::AttachmentDesc& colorAttachment = outInfo->ColorAttachments[0];
			colorAttachment.bClearOnLoad = true;
			colorAttachment.bStore = true;
			colorAttachment.bUseStencil = false;
			colorAttachment.Format = JE_GetRenderer()->GetSwapChainFormat();
			colorAttachment.MyMultisamplingMode = RenderState::MultisamplingMode::None;
			colorAttachment.Usage = RenderPass::UsageMode::ColorPresentable;

			outInfo->NumColorAttachments = 1;


			RenderPass::AttachmentDesc& depthAttachment = outInfo->DepthStencilAttachments[0];
			depthAttachment.bClearOnLoad = true;
			depthAttachment.bStore = false;
			depthAttachment.bUseStencil = false;
			depthAttachment.Format = JE_GetRenderer()->FindDepthFormat();
			depthAttachment.MyMultisamplingMode = RenderState::MultisamplingMode::None;
			depthAttachment.Usage = RenderPass::UsageMode::DepthStencil;

			outInfo->NumDepthAttachments = 1;


			RenderPass::Subpass& subpass = outInfo->Subpasses[0];
			subpass.BindPoint = Pipeline::Type::Graphics;
			subpass.ColorAttachmentIndices[0] = 0;
			subpass.NumColorAttachmentIndices = 1;
			subpass.DepthAttachmentIndex = 0;
			subpass.MyDependency = RenderPass::Dependency();	// No dependencies.

			outInfo->NumSubpasses = 1;
		}
			break;
		case Rendering::RenderPassCommon::Id::Custom:
		{
			// TODO: Implement.
			JE_Assert(false);
		}
			break;
		default:
			JE_Assert(false);
			break;
		}
	}
}