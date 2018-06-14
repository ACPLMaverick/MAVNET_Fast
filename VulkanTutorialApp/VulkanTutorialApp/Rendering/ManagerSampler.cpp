#include "ManagerSampler.h"


namespace Rendering
{
	ManagerSampler::ManagerSampler()
	{
	}


	ManagerSampler::~ManagerSampler()
	{
		JE_Assert(_samplers.size() == 0);
		JE_Assert(_samplerMap.size() == 0);
	}

	Sampler* ManagerSampler::TryGetSampler(const Sampler::Options* options)
	{
		auto it = _samplerMap.find(*options);
		if (it != _samplerMap.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}

	Sampler* ManagerSampler::GetSampler(const Sampler::Options* options)
	{
		Sampler* sampler;
		if ((sampler = TryGetSampler(options)) != nullptr)
		{
			return sampler;
		}
		else
		{
			_samplers.push_back(Sampler());
			sampler = &_samplers.back();
			_samplerMap.emplace(*options, sampler);
			sampler->Initialize(options);
			return sampler;
		}
	}

	void ManagerSampler::Initialize()
	{
		_samplers.clear();
		_samplerMap.clear();
	}

	void ManagerSampler::Cleanup()
	{
		for (auto& sampler : _samplers)
		{
			sampler.Cleanup();
		}

		_samplers.clear();
		_samplerMap.clear();
	}
}