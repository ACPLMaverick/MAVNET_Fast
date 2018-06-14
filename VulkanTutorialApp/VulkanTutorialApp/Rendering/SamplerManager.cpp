#include "SamplerManager.h"


namespace Rendering
{
	SamplerManager::SamplerManager()
	{
	}


	SamplerManager::~SamplerManager()
	{
		JE_Assert(_samplers.size() == 0);
		JE_Assert(_samplerMap.size() == 0);
	}

	Sampler* SamplerManager::TryGetSampler(const Sampler::Options* options)
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

	Sampler* SamplerManager::GetSampler(const Sampler::Options* options)
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

	void SamplerManager::Initialize()
	{
		_samplers.clear();
		_samplerMap.clear();
	}

	void SamplerManager::Cleanup()
	{
		for (auto& sampler : _samplers)
		{
			sampler.Cleanup();
		}

		_samplers.clear();
		_samplerMap.clear();
	}
}