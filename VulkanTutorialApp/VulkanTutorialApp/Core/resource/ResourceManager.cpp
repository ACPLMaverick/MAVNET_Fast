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
		::Rendering::Mesh::LoadOptions meshOptions;
		std::string postfixes[] = { "", "Quad", "Box", "Sphere" };
		for (size_t i = 1; i < (size_t)::Rendering::Mesh::AutoGenMode::ENUM_SIZE; ++i)
		{
			// TODO: Implement more autogen meshes loading.
			if(i > 1) continue;

			meshOptions.AutoGenerateMode = (::Rendering::Mesh::AutoGenMode)i;
			CacheMeshes.Get("AutoGen_" + postfixes[i], &meshOptions);
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
