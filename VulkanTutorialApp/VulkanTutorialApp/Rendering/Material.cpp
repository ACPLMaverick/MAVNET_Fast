#include "Material.h"

#include "Texture.h"

namespace Rendering
{
	Material::VertexDeclaration::VertexDeclaration()
	{
	}

	Material::VertexDeclaration::~VertexDeclaration()
	{
	}

	void Material::VertexDeclaration::Initialize(const std::vector<Material::VertexDeclaration::ComponentType>* components)
	{
		_components = *components;
	}

	bool Material::VertexDeclaration::IsHavingComponent(ComponentType type) const
	{
		for (const auto& mType : _components)
		{
			if (mType == type)
				return true;
		}

		return false;
	}

	void Material::VertexDeclaration::GetComponentSizes(std::vector<uint32_t>* sizeVector) const
	{
		sizeVector->clear();
		for (const auto& mType : _components)
		{
			sizeVector->push_back(GetComponentSize(mType));
		}
	}

	uint32_t Material::VertexDeclaration::GetComponentTotalSize() const
	{
		uint32_t sum = 0;
		for (const auto& mType : _components)
		{
			sum += GetComponentSize(mType);
		}
		return sum;
	}

	void Material::VertexDeclaration::GetBindingDescriptions(std::vector<VkVertexInputBindingDescription>* outDescriptions) const 
	{
		outDescriptions->clear();

		uint32_t bindingIndex = 0;
		for (const auto& component : _components)
		{
			VkVertexInputBindingDescription desc = {};
			desc.binding = bindingIndex;
			desc.stride = GetComponentSize(component);
			desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			outDescriptions->push_back(desc);

			++bindingIndex;
		}
	}

	void Material::VertexDeclaration::GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>* outDescriptions) const
	{
		outDescriptions->clear();

		uint32_t bindingIndex = 0;
		for (const auto& component : _components)
		{
			VkVertexInputAttributeDescription desc = {};
			desc.binding = bindingIndex;
			desc.location = bindingIndex;
			desc.format = GetComponentFormat(component);
			desc.offset = 0; // TODO: Check this when rewriting code to use only one VkBuffer for all attribs.

			outDescriptions->push_back(desc);

			++bindingIndex;
		}
	}

	bool Material::VertexDeclaration::operator==(const VertexDeclaration & other) const
	{
		if (_components.size() != other._components.size())
			return false;

		for (size_t i = 0; i < _components.size(); ++i)
		{
			if (_components[i] != other._components[i])
				return false;
		}

		return true;
	}

	Material::Material()
	{
	}


	Material::~Material()
	{
	}

	void Material::Initialize()
	{
		// TODO: Implement proper non test code.

		std::vector<VertexDeclaration::ComponentType> components;
		components.push_back(VertexDeclaration::ComponentType::Position);
		components.push_back(VertexDeclaration::ComponentType::Color);
		components.push_back(VertexDeclaration::ComponentType::Normal);
		components.push_back(VertexDeclaration::ComponentType::Uv);
		_vertexDeclaration.Initialize(&components);
	}

	void Material::Cleanup()
	{
		for (auto& tex : _textures)
		{
			tex->Cleanup();
		}
		_textures.clear();

		_vertexDeclaration = VertexDeclaration();

		_pipeline = nullptr;

		// TODO: Implement proper non test code.
	}

}