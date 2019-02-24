#include "ResourceManager.h"

namespace Core
{

	void ResourceManager::Initialize()
	{
		CacheTextures.Initialize();
		CacheShaders.Initialize();
		CacheMaterials.Initialize();
		CacheMeshes.Initialize();

		// TODO: Preload some default objects, like temp shader or white/black textures.
		::Rendering::Mesh::LoadOptions meshOptions = {};
		// ++Test
		//meshOptions.AutoGenerateOptions.SphereMode = ::Rendering::Mesh::AutoGenSphereMode::LowerHalf;
		// --Test
		for (size_t i = 1; i < (size_t)::Rendering::Mesh::AutoGenMode::ENUM_SIZE; ++i)
		{
			meshOptions.AutoGenerateMode = (::Rendering::Mesh::AutoGenMode)i;
			CacheMeshes.Get("AutoGen_" + std::string(::Rendering::Mesh::AutogenPostfixes[i]), &meshOptions);
		}
	}

	void ResourceManager::Cleanup()
	{
		CacheTextures.Cleanup();
		CacheShaders.Cleanup();
		CacheMaterials.Cleanup();
		CacheMeshes.Cleanup();
	}
}
