#pragma once

#include "Resource.h"
#include "Rendering/pipeline/Pipeline.h"
#include "VertexDeclaration.h"

namespace Rendering
{
	class Texture;
	class DescriptorSet;
	class UniformBuffer;

	class Material : public Resource
	{
	public:
		Material();
		~Material();

		void Load(const std::string& name, const ::Util::NullType* loadOptions = nullptr);
		void Update();
		void Cleanup();

		JE_Inline const VertexDeclaration* GetVertexDeclaration() const { return &_vertexDeclaration; }
		JE_Inline const DescriptorSet* GetDescriptorSet() const { return _descriptorSet; }
		JE_Inline const Pipeline* GetPipeline() const { return _pipeline; }

	protected:

		VertexDeclaration _vertexDeclaration;
		Pipeline::Key _pipelineKey;

		std::vector<Texture*> _textures;

		DescriptorSet* _descriptorSet;

		Pipeline* _pipeline;
	};
}