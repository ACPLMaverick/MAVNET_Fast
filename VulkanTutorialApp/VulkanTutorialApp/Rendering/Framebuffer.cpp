#include "Framebuffer.h"

#include "Core/HelloTriangle.h"
#include "Rendering/renderStep/RenderStep.h"
#include "Rendering/renderStep/CacheRenderStep.h"
#include "Rendering/resource/Attachment.h"

namespace Rendering
{

	Framebuffer::Framebuffer()
		: _framebuffer(nullptr)
		, _width(0)
		, _height(0)
	{

	}

	Framebuffer::~Framebuffer()
	{
		JE_Assert(!_framebuffer);
	}

	void Framebuffer::Initialize(const Info* info)
	{
		JE_Assert(info);
		JE_Assert(!_framebuffer);
		_info = *info;

		CreateVkFramebuffer();
	}

	void Framebuffer::Cleanup()
	{
		JE_Assert(_framebuffer);
		vkDestroyFramebuffer(JE_GetRenderer()->GetDevice(), _framebuffer, JE_GetRenderer()->GetAllocatorPtr());
		_framebuffer = nullptr;
	}

	void Framebuffer::CreateVkFramebuffer()
	{
		std::vector<VkImageView> allAttachments(_info.NumColorAttachments + _info.NumDepthAttachments);
		_width = 0;
		_height = 0;

		auto funcUpdateDim = [](uint32_t inDim, uint32_t& outDim)
		{
			if (outDim < inDim)
			{
				if (outDim != 0)
				{
					JE_PrintWarnLine("Not every framebuffer attachment has the same size! Assuming maximum possible dimensions.");
				}
				outDim = inDim;
			}
		};

		for (uint8_t i = 0; i < _info.NumColorAttachments; ++i)
		{
			Attachment* attachment = _info.ColorAttachments[i];
			allAttachments[i] = attachment->GetImageView();
			
			funcUpdateDim(attachment->GetInfo()->Width, _width);
			funcUpdateDim(attachment->GetInfo()->Height, _height);
		}
		for (uint8_t i = 0; i < _info.NumDepthAttachments; ++i)
		{
			Attachment* attachment = _info.DepthAttachments[i];
			JE_Assert(attachment->IsDepthStencil());
			allAttachments[i + _info.NumColorAttachments] = attachment->GetImageView();

			funcUpdateDim(attachment->GetInfo()->Width, _width);
			funcUpdateDim(attachment->GetInfo()->Height, _height);
		}

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

		RenderStep* renderStep = JE_GetRenderer()->GetCacheRenderStep()->Get(_info.AssociatedRenderStep);
		JE_Assert(renderStep);

		framebufferInfo.renderPass = renderStep->GetRenderPass()->GetVkRenderPass();

		framebufferInfo.attachmentCount = static_cast<uint32_t>(allAttachments.size());
		framebufferInfo.pAttachments = allAttachments.data();
		framebufferInfo.width = _width;
		framebufferInfo.height = _height;
		framebufferInfo.layers = 1;

		JE_AssertThrowVkResult(vkCreateFramebuffer(JE_GetRenderer()->GetDevice(), &framebufferInfo, JE_GetRenderer()->GetAllocatorPtr(), &_framebuffer));
	}
}