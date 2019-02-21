#pragma once

#include "Rendering/Manager.h"
#include "RenderPass.h"

namespace Rendering
{
	typedef uint64_t RenderPassKey;

	class ManagerRenderPass : public Manager<RenderPassKey, RenderPass, RenderPass*, RenderPass::Info>
	{
	public:
		ManagerRenderPass() : Manager() { }
		virtual ~ManagerRenderPass() { }

		virtual void Initialize() override;

	protected:

		virtual RenderPass* CreateValue(const RenderPassKey* key, const RenderPass::Info* initData) override;
		virtual RenderPass* GetValueFromWrapper(RenderPass* const* val) override
		{
			return (*val);
		}

		void FillUpInfoForRenderPassId(RenderPassCommon::Id id, RenderPass::Info* outInfo);
	};
}