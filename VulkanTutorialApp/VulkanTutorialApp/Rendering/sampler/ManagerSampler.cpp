#include "ManagerSampler.h"


namespace Rendering
{
	Sampler * ManagerSampler::CreateValue(const Sampler::Options * key, const Util::NullType * initData)
	{
		Sampler* smp = AllocateValue();
		smp->Initialize(key);
		return smp;
	}
}