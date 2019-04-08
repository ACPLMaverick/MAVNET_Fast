#pragma once

namespace Rendering
{
	namespace RenderStepCommon
	{
		JE_EnumBegin(FixedId)
			Tutorial,

			DepthStencilPrepass,
			EntityShadow,
			ForwardEntity,
			DeferredEntity,
			DeferredLighting,
			Postprocessing,
			Antialiasing,
			UserInterface
		JE_EnumEnd()

		typedef uint16_t Id;
	}
}