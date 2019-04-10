#include "RenderStepTutorial.h"

#include "Rendering/renderPass/RenderPass.h"
#include "Rendering/renderPass/ManagerRenderPass.h"
#include "Rendering/Framebuffer.h"

#include "GOM/system/Drawable.h"

#include "Core/HelloTriangle.h"

namespace Rendering
{

	void RenderStepTutorial::Initialize_Internal()
	{
		// Get RenderPass.
		{
			RenderPass::Info info = {};

			Attachment::AttachDesc& colorAttachment = info.ColorAttachments[0];
			colorAttachment.bClearOnLoad = true;
			colorAttachment.bStore = true;
			colorAttachment.bUseStencil = false;
			colorAttachment.Format = JE_GetRenderer()->GetSwapChainFormat();
			colorAttachment.MyMultisamplingMode = RenderState::MultisamplingMode::None;
			colorAttachment.Usage = Attachment::UsageMode::ColorPresentable;

			info.NumColorAttachments = 1;


			Attachment::AttachDesc& depthAttachment = info.DepthStencilAttachments[0];
			depthAttachment.bClearOnLoad = true;
			depthAttachment.bStore = false;
			depthAttachment.bUseStencil = false;
			depthAttachment.Format = JE_GetRenderer()->FindDepthFormat();
			depthAttachment.MyMultisamplingMode = RenderState::MultisamplingMode::None;
			depthAttachment.Usage = Attachment::UsageMode::DepthStencil;

			info.NumDepthAttachments = 1;


			RenderPass::Subpass& subpass = info.Subpasses[0];
			subpass.BindPoint = Pipeline::Type::Graphics;
			subpass.ColorAttachmentIndices[0] = 0;
			subpass.NumColorAttachmentIndices = 1;
			subpass.DepthAttachmentIndex = 0;
			subpass.MyDependency = RenderPass::Dependency();	// No dependencies.

			info.NumSubpasses = 1;

			_renderPass = JE_GetRenderer()->GetManagerRenderPass()->Get(&info);

			_subpassesData.push_back({ 0 });
		}

		// Create depth-stencil attachment. TODO: Temporary solution. May want to create some kind of cache for attachment, just like there is one for render steps.
		{
			_depthStencil = new Attachment();
			
			Texture::CreateOptions opts;
			opts.CreationInfo.Width = JE_GetRenderer()->GetSwapChainExtent().width;
			opts.CreationInfo.Height = JE_GetRenderer()->GetSwapChainExtent().height;
			opts.CreationInfo.Format = JE_GetRenderer()->FindDepthFormat();
			opts.CreationInfo.Channels = 2;
			opts.CreationInfo.bTransferable = false;
			opts.bClearOnCreate = false;
			opts.bClearOnLoad = true;
			opts.bGenerateMips = false;
			opts.bCPUImmutable = true;
			opts.bWriteOnly = true;
			
			_depthStencil->Create(&opts);
		}

		// Build framebuffers with attachments acquired from the swap chain.
		RebuildFramebuffers();
	}

	void RenderStepTutorial::Cleanup_Internal()
	{
		JE_CleanupDelete(_depthStencil);
	}

	void RenderStepTutorial::BeginRenderPass_Internal()
	{
		// TODO: No vulkan code must be present on this level.

		// Fill command buffer with PRE commands.

		VkCommandBuffer cmd = JE_GetRenderer()->GetCmd();
		GOM::World* world = JE_GetApp()->GetWorld();

		std::array<VkClearValue, 2> clearValues = {};
		const glm::vec4* clearColor = world->GetGlobalParameters()->GetClearColor();
		clearValues[0] = { clearColor->r, clearColor->g, clearColor->b, clearColor->a };
		clearValues[1] = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _renderPass->GetVkRenderPass();
		renderPassInfo.framebuffer = _framebuffers[JE_GetRenderer()->GetCurrentSwapChainImageIndex()]->GetVkFramebuffer();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = JE_GetRenderer()->GetSwapChainExtent();
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

#if JE_TEST_DYNAMIC_CMD_BUFFER
		vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
#else
		vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);	// For executing per-object command buffers.
#endif

		// TODO: Support push constants.
		//Rendering::UboCommon::SceneGlobal pco;
		//pco.FogColor = *_fog.GetColor();
		//pco.FogDepthNear = _fog.GetStartDepth();
		//pco.FogDepthFar = _fog.GetEndDepth();
		//pco.LightColor = *_lightDirectional.GetColor();
		//pco.LightDirectionV = *_lightDirectional.GetDirectionV();
		//vkCmdPushConstants(cmd, _pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Rendering::UboCommon::SceneGlobal), &pco);
	}

	void RenderStepTutorial::BeginSubpass_Internal(PerSubpassData& subpassData)
	{
	}

	void RenderStepTutorial::DrawSubpass_Internal(PerSubpassData& subpassData)
	{
		// There is only one subpass in this RenderStep.

		JE_Assert(subpassData.SubIdx == 0);

		// Simply issue to draw drawables to swapchain framebuffer.
		GOM::Drawable::GetBehaviour()->Draw();
	}

	void RenderStepTutorial::EndSubpass_Internal(PerSubpassData& subpassData)
	{
	}

	void RenderStepTutorial::EndRenderPass_Internal()
	{
		// TODO: No vulkan code must be present on this level.
		VkCommandBuffer cmd = JE_GetRenderer()->GetCmd();
		vkCmdEndRenderPass(cmd);
	}

	void RenderStepTutorial::OnSwapChainResize_Internal()
	{
		uint32_t newWidth = JE_GetRenderer()->GetSwapChainExtent().width;
		uint32_t newHeight = JE_GetRenderer()->GetSwapChainExtent().height;

		// Resize depth stencil attachment.
		{
			Texture::ResizeInfo resizeInfo;
			resizeInfo.Width = newWidth;
			resizeInfo.Height = newHeight;
			resizeInfo.bKeepContents = false;
			_depthStencil->Resize(&resizeInfo);
		}

		// Rebuild framebuffers with newly acquired swap chain attachments.
		RebuildFramebuffers();
	}

	void RenderStepTutorial::RebuildFramebuffers()
	{
		// Destroy current framebuffers if necessary.
		if (!_framebuffers.empty())
		{
			for (Framebuffer* framebuffer : _framebuffers)
			{
				JE_CleanupDelete(framebuffer);
			}
			_framebuffers.clear();
		}

		// Create framebuffers : for each swap chain image create framebuffer with this image and depth stencil attachment.

		const std::vector<Attachment*>& attachments = JE_GetRenderer()->GetSwapChainAttachments();
		size_t fbNum = attachments.size();

		for (size_t i = 0; i < fbNum; ++i)
		{
			Framebuffer* framebufer = new Framebuffer();

			Framebuffer::Info fbInfo;
			fbInfo.AssociatedRenderStep = _id;

			fbInfo.NumColorAttachments = 1;
			fbInfo.ColorAttachments[0] = attachments[i];

			fbInfo.NumDepthAttachments = 1;
			fbInfo.DepthAttachments[0] = _depthStencil;

			framebufer->Initialize(&fbInfo);
			_framebuffers.push_back(framebufer);
		}
	}

}