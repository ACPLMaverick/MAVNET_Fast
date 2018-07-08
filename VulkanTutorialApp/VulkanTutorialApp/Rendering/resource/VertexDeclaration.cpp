#include "VertexDeclaration.h"

namespace Rendering
{
	VertexDeclaration::VertexDeclaration()
	{
	}

	VertexDeclaration::~VertexDeclaration()
	{
	}

	void VertexDeclaration::Initialize(const std::vector<VertexDeclaration::ComponentType>* components)
	{
		_components = *components;
	}

	bool VertexDeclaration::IsHavingComponent(ComponentType type) const
	{
		for (const auto& mType : _components)
		{
			if (mType == type)
				return true;
		}

		return false;
	}

	void VertexDeclaration::GetComponentSizes(std::vector<uint32_t>* sizeVector) const
	{
		sizeVector->clear();
		for (const auto& mType : _components)
		{
			sizeVector->push_back(GetComponentSize(mType));
		}
	}

	uint32_t VertexDeclaration::GetComponentTotalSize() const
	{
		uint32_t sum = 0;
		for (const auto& mType : _components)
		{
			sum += GetComponentSize(mType);
		}
		return sum;
	}

	void VertexDeclaration::GetBindingDescriptions(std::vector<VkVertexInputBindingDescription>* outDescriptions) const
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

	void VertexDeclaration::GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>* outDescriptions) const
	{
		outDescriptions->clear();

		uint32_t bindingIndex = 0;
		for (const auto& component : _components)
		{
			VkVertexInputAttributeDescription desc = {};
			desc.binding = bindingIndex;
			desc.location = bindingIndex;
			desc.format = GetComponentFormat(component);
			desc.offset = 0; // TODO: Check this when rewriting code to use only one VkBuffer for all attributes.

			outDescriptions->push_back(desc);

			++bindingIndex;
		}
	}

	bool VertexDeclaration::operator==(const VertexDeclaration & other) const
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
}