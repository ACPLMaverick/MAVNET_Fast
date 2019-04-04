#pragma once

#include "Rendering/Manager.h"
#include "RenderPass.h"

namespace Rendering
{
	typedef RenderPass::Info RenderPassKey;

	class ManagerRenderPass : public Manager<RenderPassKey, RenderPass, RenderPass*>
	{
	public:
		ManagerRenderPass() : Manager() { }
		virtual ~ManagerRenderPass() { }

	protected:

		virtual RenderPass* CreateValue(const RenderPassKey* key, const Util::NullType* initData) override;
		virtual RenderPass* GetValueFromWrapper(RenderPass* const* val) override
		{
			return (*val);
		}
	};
}