#pragma once

#include "Resource.h"
#include "Rendering/pipeline/Pipeline.h"
#include "VertexDeclaration.h"
#include "Rendering/descriptor/DescriptorCommon.h"

namespace Rendering
{
	class Texture;
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
		JE_Inline const DescriptorCommon::LayoutData* GetDescriptorLayout() const { return &_descriptorLayout; }
		JE_Inline const Pipeline* GetPipeline() const { return _pipeline; }
		JE_Inline const Texture* GetTexture(size_t index) { JE_Assert(index < _textures.size()); return _textures[index]; }

	protected:

		VertexDeclaration _vertexDeclaration;
		Pipeline::Key _pipelineKey;

		// TODO: Float4, Float2, Float parameters...
		std::vector<Texture*> _textures;

		DescriptorCommon::LayoutData _descriptorLayout;

		Pipeline* _pipeline;
	};
}