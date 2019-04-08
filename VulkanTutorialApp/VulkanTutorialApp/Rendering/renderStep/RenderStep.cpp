#include "RenderStep.h"

#include "Core/HelloTriangle.h"
#include "Rendering/Framebuffer.h"

namespace Rendering
{
	void RenderStep::Perform()
	{
		JE_GetRenderer()->SetActiveRenderStep(this);

		BeginRenderPass();
		
		for (PerSubpassData& subpass : _subpassesData)
		{
			_currentSubpassDataIndex = subpass.SubIdx;
			BeginSubpass(subpass);
			DrawSubpass(subpass);
			EndSubpass(subpass);
		}

		_currentSubpassDataIndex = 0;

		EndRenderPass();

		JE_GetRenderer()->SetActiveRenderStep(nullptr);
	}

	void RenderStep::Initialize()
	{
		_currentSubpassDataIndex = 0;

		Initialize_Internal();
	}

	void RenderStep::Cleanup()
	{
		Cleanup_Internal();

		for (Framebuffer* framebuffer : _framebuffers)
		{
			framebuffer->Cleanup();
			delete framebuffer;
		}
		_framebuffers.clear();

		for (PerSubpassData& subpass : _subpassesData)
		{
			// TODO..?
		}
		_subpassesData.clear();
		_renderPass = nullptr;
	}

	void RenderStep::BeginRenderPass()
	{
		BeginRenderPass_Internal();
	}

	void RenderStep::BeginSubpass(PerSubpassData& subpassData)
	{
		BeginSubpass_Internal(subpassData);
	}

	void RenderStep::DrawSubpass(PerSubpassData& subpassData)
	{
		DrawSubpass_Internal(subpassData);
	}

	void RenderStep::EndSubpass(PerSubpassData& subpassData)
	{
		EndSubpass_Internal(subpassData);
	}

	void RenderStep::EndRenderPass()
	{
		EndRenderPass_Internal();
	}

}