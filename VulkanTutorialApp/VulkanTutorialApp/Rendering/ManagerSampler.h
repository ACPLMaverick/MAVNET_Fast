#pragma once

#include "Sampler.h"

namespace Rendering
{
	class ManagerSampler
	{
	public:
		ManagerSampler();
		~ManagerSampler();

		// Will return nullptr if sampler does not exist.
		Sampler* TryGetSampler(const Sampler::Options* options);

		// Will create new sampler if requested sampler does not exist.
		Sampler* GetSampler(const Sampler::Options* options);

		void Initialize();
		void Cleanup();

	private:

		std::vector<Sampler> _samplers;
		std::unordered_map<Sampler::Options, Sampler*> _samplerMap;

	};
}