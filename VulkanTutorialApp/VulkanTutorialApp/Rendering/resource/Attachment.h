#pragma once

#include "Texture.h"

#include "Rendering/pipeline/RenderState.h"

namespace Rendering
{
	class Attachment : public Texture
	{
	public:

		JE_EnumBegin(UsageMode)
			Color
			, ColorPresentable
			, Transferable
			, DepthStencil
		JE_EnumEnd()

		struct AttachDesc
		{
			uint32_t Format = {};
			RenderState::MultisamplingMode MyMultisamplingMode : 2;
			UsageMode Usage : 2;
			bool bClearOnLoad : 1;
			bool bStore : 1;
			bool bUseStencil : 1;
		};

	public:

		virtual void Resize(const ResizeInfo* resizeInfo) override;
	};
}