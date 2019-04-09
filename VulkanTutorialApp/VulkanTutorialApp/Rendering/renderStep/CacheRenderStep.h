#pragma once

#include "RenderStepCommon.h"

namespace Rendering
{
	class RenderStep;

	class CacheRenderStep
	{
	public:

		void Initialize();
		void Cleanup();
		void OnSwapChainResize();

		JE_Inline RenderStep* Get(RenderStepCommon::FixedId id) const 
		{ 
			RenderStep* step = _renderStepsFixed[(size_t)id];
			return step; 
		}

		JE_Inline RenderStep* Get(RenderStepCommon::Id id) const
		{
			return Get((RenderStepCommon::FixedId)id);
		}

	private:

		template <class StepType> void InitFixedRenderStep(RenderStepCommon::FixedId id)
		{
			StepType* step = new StepType();
			_renderStepsFixed[(size_t)RenderStepCommon::FixedId::Tutorial] = step;

			step->_id = (RenderStepCommon::Id)id;
			step->Initialize();
		}

		RenderStep * _renderStepsFixed[(size_t)RenderStepCommon::FixedId::ENUM_SIZE] = {};
	};
}