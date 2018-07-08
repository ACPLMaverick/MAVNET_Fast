#pragma once

#include "Rendering/Manager.h"

#include "Pipeline.h"
#include "Rendering/descriptor/DescriptorCommon.h"

namespace Rendering
{
	class Shader;
	class VertexDecl;

	class ManagerPipeline : public Manager<Pipeline::Key, Pipeline, Pipeline*, Pipeline::Info>
	{
	public:
		ManagerPipeline() : Manager() { }
		~ManagerPipeline() { }

	protected:

		virtual Pipeline* CreateValue(const Pipeline::Key* key, const Pipeline::Info* initData) override;
		virtual bool IsValidValueWrapper(Pipeline* const* val) override
		{
			return (*val) != nullptr;
		}
	};
}