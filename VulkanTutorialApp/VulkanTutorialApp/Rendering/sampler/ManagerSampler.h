#pragma once

#include "Rendering/Manager.h"
#include "Sampler.h"

namespace Rendering
{
	class ManagerSampler : public Manager<Sampler::Options, Sampler, Sampler*>
	{
	public:
		ManagerSampler() : Manager() { }
		virtual ~ManagerSampler() { }

	protected:

		virtual Sampler* CreateValue(const Sampler::Options* key, const Util::NullType* initData = nullptr) override;
		virtual bool IsValidValueWrapper(Sampler* const* val) override
		{
			return (*val) != nullptr;
		}
	};
}