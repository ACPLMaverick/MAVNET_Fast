#include "RenderPass.h"

#include "Core/HelloTriangle.h"

namespace Rendering
{
	RenderPass::~RenderPass()
	{
		JE_Assert(_renderPass == VK_NULL_HANDLE);
	}

	void RenderPass::Initialize(const Info * info)
	{
		JE_Assert(info != nullptr);
		JE_Assert(_renderPass == VK_NULL_HANDLE);
		JE_Assert(info->Subpasses.size() > 0);

		_info = *info;

		// TODO: extract informations based on ptrs to render targets.

		// Create render pass from info.
		std::vector<VkAttachmentDescription> attachmentDescs;

		for (size_t i = 0; i < _info.ColorAttachments.size(); ++i)
		{
			VkAttachmentDescription desc = {};
			desc.format = _info.ColorAttachments[i].Format;
			desc.samples = RenderState::ConvertToVkSampleCount(_info.ColorAttachments[i].MyMultisamplingMode);
			desc.loadOp = _info.ColorAttachments[i].bClearOnLoad ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;	   // TODO: verify if correct.
			desc.storeOp = _info.ColorAttachments[i].bStore ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
			if (_info.ColorAttachments[i].bUseStencil)
			{
				desc.stencilLoadOp = desc.loadOp;
				desc.stencilStoreOp = desc.storeOp;
			}
			else
			{
				desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			}
			desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			switch (_info.ColorAttachments[i].Usage)
			{
			case UsageMode::Color:
			default:
				desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				break;
			case UsageMode::ColorPresentable:
				desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				break;
			case UsageMode::Transferable:
				desc.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				break;
			case UsageMode::DepthStencil:
				desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				break;
			}

			attachmentDescs.push_back(desc);
		}


		const bool bUseDepth = _info.DepthStencilAttachments.size() > 0;

		for (size_t i = 0; i < _info.DepthStencilAttachments.size(); ++i)
		{
			VkAttachmentDescription depthAttachmentDesc = {};

			depthAttachmentDesc.format = Core::HelloTriangle::GetInstance()->FindDepthFormat();
			depthAttachmentDesc.samples = RenderState::ConvertToVkSampleCount(_info.DepthStencilAttachments[i].MyMultisamplingMode);

			depthAttachmentDesc.loadOp = _info.DepthStencilAttachments[i].bClearOnLoad ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;	   // TODO: verify if correct.
			depthAttachmentDesc.storeOp = _info.DepthStencilAttachments[i].bStore ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
			if (_info.DepthStencilAttachments[i].bUseStencil)
			{
				depthAttachmentDesc.stencilLoadOp = depthAttachmentDesc.loadOp;
				depthAttachmentDesc.stencilStoreOp = depthAttachmentDesc.storeOp;
			}
			else
			{
				depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			}
			depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachmentDescs.push_back(depthAttachmentDesc);
		}


		std::vector<std::vector<VkAttachmentReference>> colorAttachmentRefsForEachSubpass;
		std::vector<VkAttachmentReference> depthAttachmentRefsForEachSubpass;
		std::vector<VkSubpassDescription> subpassDescs;

		for (size_t i = 0; i < _info.Subpasses.size(); ++i)
		{
			JE_Assert(_info.Subpasses[i].ColorAttachmentIndices.size() <= RenderState::MAX_COLOR_FRAMEBUFFERS_ATTACHED);

			colorAttachmentRefsForEachSubpass.push_back(std::vector<VkAttachmentReference>());
			std::vector<VkAttachmentReference>& colorAttachmentRefs = colorAttachmentRefsForEachSubpass.back();

			for (size_t j = 0; j < _info.Subpasses[i].ColorAttachmentIndices.size(); ++j)
			{
				VkAttachmentReference attachmentRef = {};
				attachmentRef.attachment = _info.Subpasses[i].ColorAttachmentIndices[j];
				switch (_info.ColorAttachments[_info.Subpasses[i].ColorAttachmentIndices[j]].Usage)	// TODO: Do this based on real framebuffer's layout.
				{
				case UsageMode::Color:
				case UsageMode::ColorPresentable:
				case UsageMode::Transferable:
				default:
					attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					break;
				case UsageMode::DepthStencil:
					attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					break;
				}

				colorAttachmentRefs.push_back(attachmentRef);
			}


			VkAttachmentReference depthAttachmentRef = {};
			if (bUseDepth)
			{
				depthAttachmentRef.attachment = (uint32_t)(_info.ColorAttachments.size() + _info.Subpasses[i].DepthAttachmentIndex); // "next" attachment index after all colors
				depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
			depthAttachmentRefsForEachSubpass.push_back(depthAttachmentRef);


			VkSubpassDescription desc = {};
			desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			desc.colorAttachmentCount = (uint32_t)colorAttachmentRefs.size();
			desc.pColorAttachments = colorAttachmentRefs.data();
			desc.pDepthStencilAttachment = bUseDepth ? &depthAttachmentRef : nullptr;

			subpassDescs.push_back(desc);
		}


		std::vector<VkSubpassDependency> subpassDeps;

		if (_info.Dependencies.size() == 0)
		{
			VkSubpassDependency dep = {};
			dep.srcSubpass = VK_SUBPASS_EXTERNAL;
			dep.dstSubpass = 0;
			dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dep.srcAccessMask = 0;
			dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			// TODO: Are these default settings correct?

			subpassDeps.push_back(dep);
		}
		else
		{
			for (size_t i = 0; i < _info.Dependencies.size(); ++i)
			{
				VkSubpassDependency dep = {};

				dep.srcSubpass = _info.Dependencies[i].SubpassIndexSource;
				dep.dstSubpass = _info.Dependencies[i].SubpassIndexDest;
				dep.srcStageMask = _info.Dependencies[i].StageMaskSource;
				dep.srcAccessMask = _info.Dependencies[i].AccessMaskSource;
				dep.dstStageMask = _info.Dependencies[i].StageMaskDest;
				dep.dstAccessMask = _info.Dependencies[i].AccessMaskDest;
				dep.dependencyFlags = _info.Dependencies[i].Flags;

				subpassDeps.push_back(dep);
			}
		}

		
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescs.size());
		renderPassInfo.pAttachments = attachmentDescs.data();
		renderPassInfo.subpassCount = static_cast<uint32_t>(subpassDescs.size());
		renderPassInfo.pSubpasses = subpassDescs.data();
		renderPassInfo.dependencyCount = static_cast<uint32_t>(subpassDeps.size());
		renderPassInfo.pDependencies = subpassDeps.data();

		JE_AssertThrowVkResult(vkCreateRenderPass(JE_GetRenderer()->GetDevice(), &renderPassInfo, JE_GetRenderer()->GetAllocatorPtr(), &_renderPass));
	}

	void RenderPass::Cleanup()
	{
		JE_Assert(_renderPass != VK_NULL_HANDLE);

		vkDestroyRenderPass(JE_GetRenderer()->GetDevice(), _renderPass, JE_GetRenderer()->GetAllocatorPtr());
		_renderPass = VK_NULL_HANDLE;
	}
}