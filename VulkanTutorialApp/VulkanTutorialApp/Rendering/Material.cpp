#include "Material.h"

namespace Rendering
{
	size_t Material::VertexDeclaration::GetComponentSize()
	{
		return size_t();
	}

	Material::VertexDeclaration::VertexDeclaration()
	{
	}

	Material::VertexDeclaration::VertexDeclaration(const VertexDeclaration & copy)
	{
	}

	Material::VertexDeclaration::~VertexDeclaration()
	{
	}

	void Material::VertexDeclaration::Init(const std::vector<Type>* components)
	{
	}

	bool Material::VertexDeclaration::IsHavingComponent() const
	{
		return false;
	}

	const std::vector<Material::VertexDeclaration::Type>* Material::VertexDeclaration::GetComponents() const
	{
		return nullptr;
	}

	void Material::VertexDeclaration::GetComponentSizes(std::vector<size_t>* sizeVector) const
	{
	}

	void Material::VertexDeclaration::GetBindingDescription(VkVertexInputBindingDescription & outDescription)
	{
	}

	void Material::VertexDeclaration::GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& outDescriptions)
	{
	}

	bool Material::VertexDeclaration::operator==(const VertexDeclaration & other) const
	{
		return false;
	}

	Material::Material()
	{
	}


	Material::~Material()
	{
	}

}