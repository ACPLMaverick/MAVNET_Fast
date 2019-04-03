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
		JE_Assert(info->NumSubpasses > 0);
		JE_Assert(info->NumColorAttachments > 0 || info->NumDepthAttachments > 0);

		_info = *info;

		// TODO: extract informations based on ptrs to render targets.

		// Create render pass from info.
		std::vector<VkAttachmentDescription> attachmentDescs;

		for (uint8_t i = 0; i < _info.NumColorAttachments; ++i)
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


		const bool bUseDepth = _info.NumDepthAttachments > 0;

		for (uint8_t i = 0; i < _info.NumDepthAttachments; ++i)
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
		std::vector<VkSubpassDependency> subpassDeps;

		for (uint8_t i = 0; i < _info.NumSubpasses; ++i)
		{
			// TODO !!!!!!!!!! !!!!!!!!!!!!!!! !!!!!!!!!!!!!!
			JE_Assert(_info.Subpasses[i].NumColorAttachmentIndices <= MAX_ATTACHMENTS);

			colorAttachmentRefsForEachSubpass.push_back(std::vector<VkAttachmentReference>());
			std::vector<VkAttachmentReference>& colorAttachmentRefs = colorAttachmentRefsForEachSubpass.back();

			for (uint8_t j = 0; j < _info.Subpasses[i].NumColorAttachmentIndices; ++j)
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
				depthAttachmentRef.attachment = (uint32_t)(_info.NumColorAttachments + _info.Subpasses[i].DepthAttachmentIndex); // "next" attachment index after all colors
				depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
			depthAttachmentRefsForEachSubpass.push_back(depthAttachmentRef);


			VkSubpassDescription desc = {};
			desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			desc.colorAttachmentCount = (uint32_t)colorAttachmentRefs.size();
			desc.pColorAttachments = colorAttachmentRefs.data();
			desc.pDepthStencilAttachment = bUseDepth ? &depthAttachmentRef : nullptr;

			subpassDescs.push_back(desc);


			VkSubpassDependency dep = {};

			dep.srcSubpass = _info.Subpasses[i].MyDependency.SubpassIndexSource;
			dep.dstSubpass = _info.Subpasses[i].MyDependency.SubpassIndexDest;
			dep.srcStageMask = _info.Subpasses[i].MyDependency.StageMaskSource;
			dep.srcAccessMask = _info.Subpasses[i].MyDependency.AccessMaskSource;
			dep.dstStageMask = _info.Subpasses[i].MyDependency.StageMaskDest;
			dep.dstAccessMask = _info.Subpasses[i].MyDependency.AccessMaskDest;
			dep.dependencyFlags = _info.Subpasses[i].MyDependency.Flags;

			subpassDeps.push_back(dep);
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

	void RenderPass::Reinitialize()
	{
		Info tempInfo = _info;
		Cleanup();
		Initialize(&tempInfo);
	}

	RenderPass::Dependency::Dependency()
		: SubpassIndexSource(VK_SUBPASS_EXTERNAL)
		, SubpassIndexDest(0)
		, StageMaskSource(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
		, AccessMaskSource(0)
		, StageMaskDest(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
		, AccessMaskDest(VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
		, Flags(0)
	{

	}

}