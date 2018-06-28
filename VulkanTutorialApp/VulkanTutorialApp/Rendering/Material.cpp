#include "Material.h"

#include "Core/HelloTriangle.h"
#include "Texture.h"
#include "UniformBuffer.h"

#include "ManagerDescriptor.h"
#include "DescriptorSet.h"

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
		: _descriptorSet(nullptr)
		, _uboPerObject(nullptr)
		, _uboPerMaterial(nullptr)
	{
	}


	Material::~Material()
	{
	}

	void Material::Initialize()
	{
		// TODO: Implement proper non test code.

		// This is created via Shader Reflection.
		std::vector<VertexDeclaration::ComponentType> components;
		components.push_back(VertexDeclaration::ComponentType::Position);
		components.push_back(VertexDeclaration::ComponentType::Color);
		components.push_back(VertexDeclaration::ComponentType::Normal);
		components.push_back(VertexDeclaration::ComponentType::Uv);
		_vertexDeclaration.Initialize(&components);

		// This too.
		UniformBuffer::Options options;
		options.DataSize = sizeof(UboCommon::StaticMeshCommon);
		_uboPerObject = new UniformBuffer();
		_uboPerObject->Initialize(&options);

		options.DataSize = sizeof(UboCommon::SceneGlobal);
		_uboPerMaterial = new UniformBuffer();
		_uboPerMaterial->Initialize(&options);

		// This as well.
		::Rendering::Texture::LoadOptions texOptions;
		Texture* texture = new Texture();
		std::string texName = "chalet.jpg";
		texture->Initialize(&texName, &texOptions);
		_textures.push_back(texture);

		//This is created based on material properties (TODO)
		DescriptorSet::Info info;
		info.LayInfo.Bindings[0] = DescriptorCommon::LayoutInfo::Binding
		(
			DescriptorCommon::ShaderStage::Vertex,
			0,
			1,
			ResourceCommon::Type::UniformBuffer
		);

		info.LayInfo.Bindings[1] = DescriptorCommon::LayoutInfo::Binding
		(
			DescriptorCommon::ShaderStage::Fragment,
			1,
			1,
			ResourceCommon::Type::Texture2D
		);
		info.Resources[0][0] = _uboPerObject;
		info.Resources[1][0] = _textures[0];
		_descriptorSet = JE_GetRenderer()->GetManagerDescriptor()->CreateDescriptorSet(&info);
	}

	void Material::Update()
	{
		UpdateUboPerObject();
		UpdateUboPerMaterial();
	}

	void Material::Cleanup()
	{
		for (auto& tex : _textures)
		{
			tex->Cleanup();
			delete tex;	// TODO: Do not remove, instead make manager do this.
		}
		_textures.clear();

		_vertexDeclaration = VertexDeclaration();

		if (_uboPerObject != nullptr)	// TODO: Do not remove, instead make manager do this.
		{
			_uboPerObject->Cleanup();
			delete _uboPerObject;
			_uboPerObject = nullptr;
		}
		if (_uboPerMaterial != nullptr)	// TODO: Do not cleanup, instead make manager do this.
		{
			_uboPerMaterial->Cleanup();
			delete _uboPerMaterial;
			_uboPerMaterial = nullptr;
		}

		_pipeline = nullptr;

		// TODO: Implement proper non test code.
	}

	void Material::UpdateUboPerObject()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UboCommon::StaticMeshCommon matrices;

		glm::mat4 mv_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		mv_translation = mv_translation * rotation * scale;

		matrices.MVP = *JE_GetRenderer()->GetCamera()->GetViewProj() * mv_translation;
		matrices.MV = mv_translation;
		matrices.MVInverseTranspose = glm::transpose(glm::inverse(mv_translation));

		_uboPerObject->UpdateWithData(reinterpret_cast<uint8_t*>(&matrices), sizeof(matrices));
	}

	void Material::UpdateUboPerMaterial()
	{
	}

}