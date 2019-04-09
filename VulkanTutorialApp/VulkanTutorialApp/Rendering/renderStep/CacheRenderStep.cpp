#include "CacheRenderStep.h"

#include "RenderStep.h"
#include "RenderStepTutorial.h"

namespace Rendering
{

	void CacheRenderStep::Initialize()
	{
		InitFixedRenderStep<RenderStepTutorial>(RenderStepCommon::FixedId::Tutorial);
		// TODO: Initialize all steps...
	}

	void CacheRenderStep::Cleanup()
	{
		for (size_t i = 0; i < (size_t)RenderStepCommon::FixedId::ENUM_SIZE; ++i)
		{
			if (_renderStepsFixed[i])
			{
				_renderStepsFixed[i]->Cleanup();
				delete _renderStepsFixed[i];
				_renderStepsFixed[i] = nullptr;
			}
		}
	}

	void CacheRenderStep::OnSwapChainResize()
	{
		for (size_t i = 0; i < (size_t)RenderStepCommon::FixedId::ENUM_SIZE; ++i)
		{
			if (_renderStepsFixed[i])
			{
				_renderStepsFixed[i]->OnSwapChainResize();
			}
		}
	}

}