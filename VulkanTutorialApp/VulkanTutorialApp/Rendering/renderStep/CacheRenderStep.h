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

		JE_Inline RenderStep* Get(RenderStepCommon::FixedId id) const 
		{ 
			RenderStep* step = _renderStepsFixed[(size_t)id];
			JE_Assert(step);
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
			step->_id = id;
			step->Initialize();
			_renderStepsFixed[(size_t)RenderStepCommon::FixedId::Tutorial] = step;
		}

		RenderStep * _renderStepsFixed[(size_t)RenderStepCommon::FixedId::ENUM_SIZE] = {};
	};
}