#include "RenderStepTutorial.h"

#include "Rendering/renderPass/RenderPass.h"
#include "Rendering/renderPass/ManagerRenderPass.h"

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
		renderPassInfo.framebuffer = /*_swapChainFramebuffers[_imageIndex]*/ nullptr;	// TODOTODO !!!!!!!!!!!!!!!!!!
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

}