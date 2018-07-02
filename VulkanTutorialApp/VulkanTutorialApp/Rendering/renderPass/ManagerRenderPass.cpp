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
			RenderPass::Attachment colorAttachment = {};
			colorAttachment.bClearOnLoad = true;
			colorAttachment.bStore = true;
			colorAttachment.bUseStencil = false;
			colorAttachment.Format = JE_GetRenderer()->GetSwapChainFormat();
			colorAttachment.MyMultisamplingMode = RenderState::MultisamplingMode::None;
			colorAttachment.Usage = RenderPass::UsageMode::ColorPresentable;
			outInfo->ColorAttachments.push_back(colorAttachment);

			RenderPass::Attachment depthAttachment = {};
			depthAttachment.bClearOnLoad = true;
			colorAttachment.bStore = false;
			colorAttachment.bUseStencil = false;
			colorAttachment.Format = JE_GetRenderer()->FindDepthFormat();
			colorAttachment.MyMultisamplingMode = RenderState::MultisamplingMode::None;
			colorAttachment.Usage = RenderPass::UsageMode::DepthStencil;
			outInfo->DepthStencilAttachments.push_back(depthAttachment);

			RenderPass::Subpass subpass;
			subpass.BindPoint = Pipeline::Type::Graphics;
			subpass.ColorAttachmentIndices.push_back(0);
			subpass.DepthAttachmentIndex = 0;
			outInfo->Subpasses.push_back(subpass);

			// No dependencies.
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