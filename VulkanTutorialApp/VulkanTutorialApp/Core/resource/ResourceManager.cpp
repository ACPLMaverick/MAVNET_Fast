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

		// Load autogen meshes.
		{
			using namespace ::Rendering;
			Mesh::LoadOptions meshOptions = {};

			for (size_t i = 1; i < (size_t)Mesh::AutoGenMode::ENUM_SIZE; ++i)
			{
				meshOptions.AutoGenerateMode = (Mesh::AutoGenMode)i;
				CacheMeshes.Get("AutoGen_" + std::string(Mesh::AutogenPostfixes[i]), &meshOptions);
			}

			meshOptions.AutoGenerateMode = Mesh::AutoGenMode::Sphere;
			meshOptions.AutoGenerateOptions.SphereMode = Mesh::AutoGenSphereMode::LowerHalf;
			CacheMeshes.Get("AutoGen_" + std::string(Mesh::AutogenPostfixes[(size_t)Mesh::AutoGenMode::Sphere] + std::string("_") + std::string(Mesh::AutogenSphereModePostfixes[(size_t)meshOptions.AutoGenerateOptions.SphereMode])), &meshOptions);

			meshOptions.AutoGenerateOptions.SphereMode = Mesh::AutoGenSphereMode::UpperHalf;
			CacheMeshes.Get("AutoGen_" + std::string(Mesh::AutogenPostfixes[(size_t)Mesh::AutoGenMode::Sphere] + std::string("_") + std::string(Mesh::AutogenSphereModePostfixes[(size_t)meshOptions.AutoGenerateOptions.SphereMode])), &meshOptions);

			meshOptions.AutoGenerateMode = Mesh::AutoGenMode::Cylinder;
			meshOptions.AutoGenerateOptions.CylinderNoCaps = true;
			CacheMeshes.Get("AutoGen_" + std::string(Mesh::AutogenPostfixes[(size_t)Mesh::AutoGenMode::Cylinder] + std::string("_") + std::string(Mesh::AutogenNoCapsPostfix)), &meshOptions);
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
