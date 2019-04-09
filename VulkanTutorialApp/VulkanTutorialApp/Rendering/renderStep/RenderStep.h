#pragma once

#include "RenderStepCommon.h"

namespace Rendering
{
	class CacheRenderStep;
	class RenderPass;
	class Pipeline;
	class Framebuffer;

	class RenderStep
	{
	protected:

		struct PerSubpassData
		{
			uint32_t SubIdx;
		};

	public:

		JE_Inline RenderStepCommon::Id GetId() const { return _id; }
		JE_Inline bool IsFixed() const { return _id < (RenderStepCommon::Id)RenderStepCommon::FixedId::ENUM_SIZE; }
		JE_Inline RenderStepCommon::FixedId GetFixedId() const { JE_Assert(IsFixed()); return (RenderStepCommon::FixedId)_id; }
		JE_Inline const RenderPass* GetRenderPass() const { return _renderPass; }
		JE_Inline uint32_t GetActiveSubpass() const { return _subpassesData[_currentSubpassDataIndex].SubIdx; }

		void Perform();

		void OnSwapChainResize();

	protected:

		RenderStep()
			: _renderPass(nullptr)
		{
		}

		virtual ~RenderStep()
		{
		}

		void Initialize();
		void Cleanup();

		void BeginRenderPass();
		void BeginSubpass(PerSubpassData& subpassData);
		void DrawSubpass(PerSubpassData& subpassData);
		void EndSubpass(PerSubpassData& subpassData);
		void EndRenderPass();

		virtual void Initialize_Internal() = 0;
		virtual void Cleanup_Internal() { };
		virtual void BeginRenderPass_Internal() = 0;
		virtual void BeginSubpass_Internal(PerSubpassData& subpassData) = 0;
		virtual void DrawSubpass_Internal(PerSubpassData& subpassData) = 0;
		virtual void EndSubpass_Internal(PerSubpassData& subpassData) = 0;
		virtual void EndRenderPass_Internal() = 0;
		virtual void OnSwapChainResize_Internal() { };

		std::vector<Framebuffer*> _framebuffers;
		std::vector<PerSubpassData> _subpassesData;
		RenderStepCommon::Id _id;
		RenderPass* _renderPass;
		size_t _currentSubpassDataIndex;

		friend class CacheRenderStep;
	};
}