#include "ManagerPipeline.h"



namespace Rendering
{
	Pipeline * ManagerPipeline::CreateValue(const Pipeline::Key * key, const Pipeline::Info* initData)
	{
		Pipeline* pipeline = AllocateValue();

		pipeline->Initialize(initData);

		return pipeline;
	}
}