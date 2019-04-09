#pragma once

#include "RenderStep.h"

namespace Rendering
{
	class Attachment;

	class RenderStepTutorial : public RenderStep
	{
	protected:

		virtual void Initialize_Internal() override;
		virtual void Cleanup_Internal() override;
		virtual void BeginRenderPass_Internal() override;
		virtual void BeginSubpass_Internal(PerSubpassData& subpassData) override;
		virtual void DrawSubpass_Internal(PerSubpassData& subpassData) override;
		virtual void EndSubpass_Internal(PerSubpassData& subpassData) override;
		virtual void EndRenderPass_Internal() override;
		virtual void OnSwapChainResize_Internal() override;

		void RebuildFramebuffers();

		Attachment* _depthStencil;
	};
}