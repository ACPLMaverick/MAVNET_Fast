#pragma once

#include "ResourceCache.h"

#include "Rendering/resource/Shader.h"
#include "Rendering/resource/Texture.h"
#include "Rendering/resource/Material.h"
#include "Rendering/resource/Mesh.h"

namespace Core
{
	class ResourceManager
	{
	public:

		void Initialize();
		void Cleanup();


	public:

		ResourceCache<::Rendering::Texture, ::Rendering::Texture::LoadOptions> CacheTextures;
		ResourceCache<::Rendering::Shader, ::Util::NullType> CacheShaders;
		ResourceCache<::Rendering::Material, ::Util::NullType> CacheMaterials;
		ResourceCache<::Rendering::Mesh, ::Rendering::Mesh::LoadOptions> CacheMeshes;
	};
}